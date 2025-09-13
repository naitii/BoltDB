#include "server.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>

Server::Server(DataStore& dataStore, PersistenceManager& persistenceManager)
    : dataStore_(dataStore), persistenceManager_(persistenceManager), 
      serverSocket_(INVALID_SOCKET_VALUE), running_(false) {
}

Server::~Server() {
    stop();
    cleanupNetworking();
}

bool Server::initializeNetworking() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return false;
    }
#endif
    return true;
}

void Server::cleanupNetworking() {
#ifdef _WIN32
    WSACleanup();
#endif
}

bool Server::start(int port) {
    if (running_) {
        std::cerr << "Server is already running" << std::endl;
        return false;
    }

    if (!initializeNetworking()) {
        return false;
    }

    // Create socket
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ == INVALID_SOCKET_VALUE) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, 
                   reinterpret_cast<const char*>(&opt), sizeof(opt)) < 0) {
        std::cerr << "Failed to set socket options" << std::endl;
        closeSocket(serverSocket_);
        return false;
    }

    // Bind socket
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverSocket_, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0) {
        std::cerr << "Failed to bind socket to port " << port << std::endl;
        closeSocket(serverSocket_);
        return false;
    }

    // Listen for connections
    if (listen(serverSocket_, 10) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        closeSocket(serverSocket_);
        return false;
    }

    running_ = true;
    std::cout << "BoltDB server started on port " << port << std::endl;

    // Accept connections in a loop
    int clientId = 0;
    while (running_) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLen = sizeof(clientAddress);
        
        socket_t clientSocket = accept(serverSocket_, 
                                     reinterpret_cast<struct sockaddr*>(&clientAddress), 
                                     &clientAddressLen);
        
        if (clientSocket == INVALID_SOCKET_VALUE) {
            if (running_) {
                std::cerr << "Failed to accept client connection" << std::endl;
            }
            continue;
        }

        // Create a new thread for this client
        std::thread clientThread(&Server::handleClient, this, clientSocket, ++clientId);
        
        {
            std::lock_guard<std::mutex> lock(threadsMutex_);
            clientThreads_.push_back(std::move(clientThread));
        }
    }

    return true;
}

void Server::stop() {
    if (!running_) {
        return;
    }

    running_ = false;
    
    // Close server socket
    closeSocket(serverSocket_);
    serverSocket_ = INVALID_SOCKET_VALUE;

    // Wait for all client threads to finish
    {
        std::lock_guard<std::mutex> lock(threadsMutex_);
        for (auto& thread : clientThreads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        clientThreads_.clear();
    }

    std::cout << "Server stopped" << std::endl;
}

bool Server::isRunning() const {
    return running_;
}

void Server::handleClient(socket_t clientSocket, int clientId) {
    std::cout << "Client " << clientId << " connected" << std::endl;
    
    char buffer[1024];
    std::string commandBuffer;
    
    while (running_) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesReceived <= 0) {
            break; // Client disconnected or error
        }
        
        buffer[bytesReceived] = '\0';
        commandBuffer += buffer;
        
        // Process complete commands (terminated by \n)
        size_t pos;
        while ((pos = commandBuffer.find('\n')) != std::string::npos) {
            std::string command = commandBuffer.substr(0, pos);
            commandBuffer.erase(0, pos + 1);
            
            // Trim whitespace
            command.erase(command.find_last_not_of(" \t\r\n") + 1);
            command.erase(0, command.find_first_not_of(" \t\r\n"));
            
            if (!command.empty()) {
                if (!processCommand(command, clientSocket)) {
                    closeSocket(clientSocket);
                    std::cout << "Client " << clientId << " disconnected" << std::endl;
                    return;
                }
            }
        }
    }
    
    closeSocket(clientSocket);
    std::cout << "Client " << clientId << " disconnected" << std::endl;
}

bool Server::processCommand(const std::string& command, socket_t clientSocket) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    // Convert to uppercase for case-insensitive commands
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    
    if (cmd == "SET") {
        std::string key, value;
        if (iss >> key) {
            // Get the rest of the line as the value
            std::string remaining;
            std::getline(iss, remaining);
            if (!remaining.empty() && remaining[0] == ' ') {
                value = remaining.substr(1); // Remove leading space
            }
            
            if (dataStore_.set(key, value)) {
                return sendResponse("+OK\n", clientSocket);
            } else {
                return sendResponse("-ERR Failed to set key\n", clientSocket);
            }
        } else {
            return sendResponse("-ERR Invalid SET command\n", clientSocket);
        }
    }
    else if (cmd == "GET") {
        std::string key;
        if (iss >> key) {
            auto value = dataStore_.get(key);
            if (value.has_value()) {
                std::string response = "$" + std::to_string(value->length()) + "\n" + *value + "\n";
                return sendResponse(response, clientSocket);
            } else {
                return sendResponse("$-1\n", clientSocket);
            }
        } else {
            return sendResponse("-ERR Invalid GET command\n", clientSocket);
        }
    }
    else if (cmd == "DELETE") {
        std::string key;
        if (iss >> key) {
            bool deleted = dataStore_.del(key);
            std::string response = ":" + std::to_string(deleted ? 1 : 0) + "\n";
            return sendResponse(response, clientSocket);
        } else {
            return sendResponse("-ERR Invalid DELETE command\n", clientSocket);
        }
    }
    else if (cmd == "QUIT") {
        return sendResponse("+OK\n", clientSocket);
    }
    else {
        return sendResponse("-ERR Unknown command: " + cmd + "\n", clientSocket);
    }
}

bool Server::sendResponse(const std::string& response, socket_t clientSocket) {
    int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
    return bytesSent > 0;
}

void Server::closeSocket(socket_t socket) {
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}
