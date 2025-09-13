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

class SimpleTestClient {
private:
    socket_t socket_;

public:
    SimpleTestClient() : socket_(INVALID_SOCKET_VALUE) {
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    }

    ~SimpleTestClient() {
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

int main() {
    std::cout << "=== BoltDB Simple Test Client ===" << std::endl;
    
    SimpleTestClient client;
    
    if (!client.connect("127.0.0.1", 7379)) {
        std::cerr << "Failed to connect to server. Make sure BoltDB server is running." << std::endl;
        return 1;
    }

    // Test basic operations quickly
    std::cout << "\n--- Testing SET operations ---" << std::endl;
    std::cout << "SET name Alice: " << client.sendCommand("SET name Alice");
    std::cout << "SET age 30: " << client.sendCommand("SET age 30");
    std::cout << "SET city New York: " << client.sendCommand("SET city New York");

    std::cout << "\n--- Testing GET operations ---" << std::endl;
    std::cout << "GET name: " << client.sendCommand("GET name");
    std::cout << "GET age: " << client.sendCommand("GET age");
    std::cout << "GET city: " << client.sendCommand("GET city");
    std::cout << "GET nonexistent: " << client.sendCommand("GET nonexistent");

    std::cout << "\n--- Testing DELETE operations ---" << std::endl;
    std::cout << "DELETE age: " << client.sendCommand("DELETE age");
    std::cout << "GET age (after delete): " << client.sendCommand("GET age");
    std::cout << "DELETE nonexistent: " << client.sendCommand("DELETE nonexistent");

    std::cout << "\n--- Testing special characters ---" << std::endl;
    std::cout << "SET message 'Hello, World!': " << client.sendCommand("SET message Hello, World!");
    std::cout << "GET message: " << client.sendCommand("GET message");

    std::cout << "\n--- Testing multiple values ---" << std::endl;
    for (int i = 1; i <= 5; ++i) {
        std::string key = "key" + std::to_string(i);
        std::string value = "value" + std::to_string(i);
        std::cout << "SET " << key << " " << value << ": " << client.sendCommand("SET " + key + " " + value);
    }

    std::cout << "\n--- Retrieving multiple values ---" << std::endl;
    for (int i = 1; i <= 5; ++i) {
        std::string key = "key" + std::to_string(i);
        std::cout << "GET " << key << ": " << client.sendCommand("GET " + key);
    }

    client.disconnect();
    std::cout << "\nTest completed successfully! Disconnected from server." << std::endl;
    
    return 0;
}

