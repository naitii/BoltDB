#include "datastore.h"
#include "persistence.h"
#include "server.h"
#include "http_server.h"
#include <iostream>
#include <signal.h>
#include <memory>
#include <filesystem>

// Global variables for signal handling
std::unique_ptr<Server> g_server;
std::unique_ptr<HttpServer> g_httpServer;
std::unique_ptr<PersistenceManager> g_persistenceManager;
std::unique_ptr<DataStore> g_dataStore;

/**
 * Signal handler for graceful shutdown
 */
void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Shutting down gracefully..." << std::endl;
    
    if (g_server) {
        g_server->stop();
    }
    if (g_httpServer) {
        g_httpServer->stop();
    }
    
    if (g_persistenceManager) {
        g_persistenceManager->stopPersistence();
        g_persistenceManager->forceSave(); // Final save before exit
    }
    
    std::cout << "BoltDB server shutdown complete." << std::endl;
    exit(0);
}

/**
 * Print usage information
 */
void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [port] [dump_file]" << std::endl;
    std::cout << "  port      - Port number to listen on (default: 7379)" << std::endl;
    std::cout << "  dump_file - Database dump file (default: dump.bdb)" << std::endl;
    std::cout << "  HTTP UI   - Available at http://localhost:8080" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  SET key value    - Store a key-value pair" << std::endl;
    std::cout << "  GET key          - Retrieve a value by key" << std::endl;
    std::cout << "  DELETE key       - Delete a key-value pair" << std::endl;
    std::cout << "  QUIT             - Disconnect from server" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== BoltDB - In-Memory Key-Value Database ===" << std::endl;
    std::cout << "Version: 1.0.0" << std::endl;
    std::cout << "Author: C++ Implementation" << std::endl;
    std::cout << std::endl;

    // Parse command line arguments
    int port = 7379;
    std::string dumpFile = "dump.bdb";
    
    if (argc > 1) {
        if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        
        try {
            port = std::stoi(argv[1]);
            if (port < 1 || port > 65535) {
                std::cerr << "Error: Port must be between 1 and 65535" << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid port number: " << argv[1] << std::endl;
            return 1;
        }
    }
    
    if (argc > 2) {
        dumpFile = argv[2];
    }

    // Set up signal handlers for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
#ifdef _WIN32
    signal(SIGBREAK, signalHandler);
#else
    signal(SIGHUP, signalHandler);
#endif

    try {
        // Create data store
        g_dataStore = std::make_unique<DataStore>();
        std::cout << "Data store initialized" << std::endl;

        // Create persistence manager
        g_persistenceManager = std::make_unique<PersistenceManager>(*g_dataStore, dumpFile);
        
        // Initialize persistence (load existing data)
        if (!g_persistenceManager->initialize()) {
            std::cerr << "Warning: Failed to initialize persistence. Starting with empty database." << std::endl;
        }

        // Start persistence thread
        g_persistenceManager->startPersistence(60); // Save every 60 seconds
        std::cout << "Persistence manager started" << std::endl;

        // Create and start server
        g_server = std::make_unique<Server>(*g_dataStore, *g_persistenceManager);
        
        // Start embedded HTTP UI server
        std::string webRoot = "web";
        if (!std::filesystem::exists(webRoot)) {
            if (std::filesystem::exists("../web")) webRoot = "../web";
            else if (std::filesystem::exists("../../web")) webRoot = "../../web";
        }
        g_httpServer = std::make_unique<HttpServer>(*g_dataStore);
        if (!g_httpServer->start(8080, webRoot)) {
            std::cerr << "Warning: Failed to start HTTP UI server" << std::endl;
        }

        std::cout << "Starting server on port " << port << "..." << std::endl;
        std::cout << "Database file: " << dumpFile << std::endl;
        std::cout << "Press Ctrl+C to stop the server" << std::endl;
        std::cout << std::endl;

        // Start the server (this will block until server stops)
        if (!g_server->start(port)) {
            std::cerr << "Failed to start server" << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
