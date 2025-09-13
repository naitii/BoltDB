#pragma once

#include "datastore.h"
#include "persistence.h"
#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <atomic>
#include <mutex>

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

/**
 * Multi-threaded TCP server for BoltDB
 * Handles client connections and command processing
 */
class Server {
private:
    DataStore& dataStore_;
    PersistenceManager& persistenceManager_;
    socket_t serverSocket_;
    std::atomic<bool> running_;
    std::vector<std::thread> clientThreads_;
    std::mutex threadsMutex_;

    /**
     * Initialize networking (Windows-specific)
     * @return true if successful, false otherwise
     */
    bool initializeNetworking();

    /**
     * Cleanup networking (Windows-specific)
     */
    void cleanupNetworking();

    /**
     * Handle a single client connection
     * @param clientSocket The client socket
     * @param clientId Unique identifier for this client
     */
    void handleClient(socket_t clientSocket, int clientId);

    /**
     * Process a command from the client
     * @param command The command string
     * @param clientSocket The client socket for response
     * @return true if connection should continue, false to disconnect
     */
    bool processCommand(const std::string& command, socket_t clientSocket);

    /**
     * Send a response to the client
     * @param response The response string
     * @param clientSocket The client socket
     * @return true if successful, false otherwise
     */
    bool sendResponse(const std::string& response, socket_t clientSocket);

    /**
     * Close a socket
     * @param socket The socket to close
     */
    void closeSocket(socket_t socket);

public:
    /**
     * Constructor
     * @param dataStore Reference to the data store
     * @param persistenceManager Reference to the persistence manager
     */
    Server(DataStore& dataStore, PersistenceManager& persistenceManager);

    /**
     * Destructor
     */
    ~Server();

    /**
     * Start the server
     * @param port Port number to listen on
     * @return true if successful, false otherwise
     */
    bool start(int port = 7379);

    /**
     * Stop the server
     */
    void stop();

    /**
     * Check if server is running
     * @return true if running, false otherwise
     */
    bool isRunning() const;
};
