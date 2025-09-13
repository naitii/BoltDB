#include <iostream>
#include <string>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using socket_t = SOCKET;
    const socket_t INVALID_SOCKET_VALUE = INVALID_SOCKET;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    using socket_t = int;
    const socket_t INVALID_SOCKET_VALUE = -1;
#endif

class InteractiveClient {
private:
    socket_t socket_;

public:
    InteractiveClient() : socket_(INVALID_SOCKET_VALUE) {
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    }

    ~InteractiveClient() {
        if (socket_ != INVALID_SOCKET_VALUE) {
#ifdef _WIN32
            closesocket(socket_);
            WSACleanup();
#else
            close(socket_);
#endif
        }
    }

    bool connect(const std::string& host, int port) {
        socket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_ == INVALID_SOCKET_VALUE) {
            std::cerr << "Failed to create socket" << std::endl;
            return false;
        }

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        
#ifdef _WIN32
        serverAddr.sin_addr.s_addr = inet_addr(host.c_str());
#else
        inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr);
#endif

        if (::connect(socket_, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
            std::cerr << "Failed to connect to server" << std::endl;
            return false;
        }

        std::cout << "Connected to BoltDB server at " << host << ":" << port << std::endl;
        return true;
    }

    std::string sendCommand(const std::string& command) {
        if (socket_ == INVALID_SOCKET_VALUE) {
            return "ERROR: Not connected";
        }

        std::string fullCommand = command + "\n";
        if (send(socket_, fullCommand.c_str(), fullCommand.length(), 0) < 0) {
            return "ERROR: Failed to send command";
        }

        char buffer[1024];
        int bytesReceived = recv(socket_, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            return "ERROR: Failed to receive response";
        }

        buffer[bytesReceived] = '\0';
        return std::string(buffer);
    }

    void disconnect() {
        if (socket_ != INVALID_SOCKET_VALUE) {
            sendCommand("QUIT");
#ifdef _WIN32
            closesocket(socket_);
#else
            close(socket_);
#endif
            socket_ = INVALID_SOCKET_VALUE;
        }
    }
};

void printHelp() {
    std::cout << "\n=== BoltDB Interactive Client ===" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  SET key value    - Store a key-value pair" << std::endl;
    std::cout << "  GET key          - Retrieve a value by key" << std::endl;
    std::cout << "  DELETE key       - Delete a key-value pair" << std::endl;
    std::cout << "  QUIT             - Disconnect and exit" << std::endl;
    std::cout << "  HELP             - Show this help message" << std::endl;
    std::cout << "  CLEAR            - Clear screen" << std::endl;
    std::cout << std::endl;
}

int main() {
    InteractiveClient client;
    
    if (!client.connect("127.0.0.1", 7379)) {
        std::cerr << "Failed to connect to server. Make sure BoltDB server is running." << std::endl;
        return 1;
    }

    printHelp();
    
    std::string command;
    while (true) {
        std::cout << "boltdb> ";
        std::getline(std::cin, command);
        
        if (command.empty()) continue;
        
        // Handle special commands
        if (command == "QUIT" || command == "quit" || command == "exit") {
            client.disconnect();
            std::cout << "Goodbye!" << std::endl;
            break;
        }
        else if (command == "HELP" || command == "help") {
            printHelp();
            continue;
        }
        else if (command == "CLEAR" || command == "clear") {
            system("cls"); // Windows
            continue;
        }
        
        // Send command to server
        std::string response = client.sendCommand(command);
        std::cout << response;
    }
    
    return 0;
}

