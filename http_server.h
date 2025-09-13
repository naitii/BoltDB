#pragma once

#include "datastore.h"
#include <atomic>
#include <string>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
using http_socket_t = SOCKET;
const http_socket_t HTTP_INVALID_SOCKET = INVALID_SOCKET;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
using http_socket_t = int;
const http_socket_t HTTP_INVALID_SOCKET = -1;
#endif

class HttpServer {
public:
    explicit HttpServer(DataStore &dataStore);
    ~HttpServer();

    bool start(int port = 8080, const std::string &webRoot = "web");
    void stop();
    bool isRunning() const;

private:
    DataStore &dataStore_;
    std::atomic<bool> running_;
    http_socket_t serverSocket_;
    std::thread acceptThread_;
    std::string webRoot_;

    void acceptLoop();
    void handleClient(http_socket_t clientSocket);
    void closeSocket(http_socket_t s);
    bool sendAll(http_socket_t s, const char *data, size_t len);

    // HTTP helpers
    static std::string urlDecode(const std::string &src);
    static std::string getHeaderValue(const std::string &headers, const std::string &key);
    static std::string getMimeType(const std::string &path);
    static std::string readFileToString(const std::string &path);
};


