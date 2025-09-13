#include "http_server.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

HttpServer::HttpServer(DataStore &dataStore)
    : dataStore_(dataStore), running_(false), serverSocket_(HTTP_INVALID_SOCKET) {}

HttpServer::~HttpServer() { stop(); }

bool HttpServer::start(int port, const std::string &webRoot) {
    if (running_) return false;
    webRoot_ = webRoot;

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }
#endif

    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ == HTTP_INVALID_SOCKET) {
        std::cerr << "HTTP: Failed to create socket" << std::endl;
        return false;
    }

    int opt = 1;
    setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(serverSocket_, (sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "HTTP: bind failed on port " << port << std::endl;
        closeSocket(serverSocket_);
        return false;
    }

    if (listen(serverSocket_, 16) < 0) {
        std::cerr << "HTTP: listen failed" << std::endl;
        closeSocket(serverSocket_);
        return false;
    }

    running_ = true;
    acceptThread_ = std::thread(&HttpServer::acceptLoop, this);
    std::cout << "HTTP server started on http://localhost:" << port << std::endl;
    return true;
}

void HttpServer::stop() {
    if (!running_) return;
    running_ = false;
    closeSocket(serverSocket_);
    if (acceptThread_.joinable()) acceptThread_.join();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool HttpServer::isRunning() const { return running_; }

void HttpServer::acceptLoop() {
    while (running_) {
        sockaddr_in client{};
        socklen_t len = sizeof(client);
        http_socket_t cs = accept(serverSocket_, (sockaddr *)&client, &len);
        if (cs == HTTP_INVALID_SOCKET) continue;
        std::thread(&HttpServer::handleClient, this, cs).detach();
    }
}

static std::string trim(const std::string &s) {
    size_t b = s.find_first_not_of(" \r\n\t");
    size_t e = s.find_last_not_of(" \r\n\t");
    if (b == std::string::npos) return "";
    return s.substr(b, e - b + 1);
}

void HttpServer::handleClient(http_socket_t clientSocket) {
    char buf[4096];
    int n = recv(clientSocket, buf, sizeof(buf) - 1, 0);
    if (n <= 0) {
        closeSocket(clientSocket);
        return;
    }
    buf[n] = '\0';
    std::string req(buf);

    // Parse request line
    std::istringstream iss(req);
    std::string method, path, version;
    iss >> method >> path >> version;

    // Basic routing
    if (method == "GET" && path.rfind("/api/get?key=", 0) == 0) {
        std::string key = urlDecode(path.substr(std::string("/api/get?key=").size()));
        auto val = dataStore_.get(key);
        std::string body;
        int status = 200;
        if (val.has_value()) {
            body = std::string("{\"key\":\"") + key + "\",\"value\":\"" + *val + "\"}";
        } else {
            status = 404;
            body = std::string("{\"error\":\"not_found\"}");
        }
        std::ostringstream res;
        res << "HTTP/1.1 " << status << " OK\r\n";
        res << "Content-Type: application/json\r\n";
        res << "Content-Length: " << body.size() << "\r\n\r\n";
        res << body;
        auto s = res.str();
        sendAll(clientSocket, s.c_str(), s.size());
        closeSocket(clientSocket);
        return;
    }

    if (method == "POST" && path == "/api/set") {
        std::string headers = req.substr(0, req.find("\r\n\r\n"));
        std::string body = req.substr(req.find("\r\n\r\n") + 4);
        // Expect body as key=value form
        std::string key, value;
        size_t kpos = body.find("key=");
        size_t vpos = body.find("&value=");
        if (kpos != std::string::npos && vpos != std::string::npos) {
            key = urlDecode(body.substr(kpos + 4, vpos - (kpos + 4)));
            value = urlDecode(body.substr(vpos + 7));
            bool ok = dataStore_.set(key, value);
            std::string resp = ok ? "{\"ok\":true}" : "{\"ok\":false}";
            std::ostringstream res;
            res << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
                << resp.size() << "\r\n\r\n" << resp;
            auto s = res.str();
            sendAll(clientSocket, s.c_str(), s.size());
            closeSocket(clientSocket);
            return;
        }
    }

    if (method == "POST" && path == "/api/delete") {
        std::string body = req.substr(req.find("\r\n\r\n") + 4);
        std::string key;
        size_t kpos = body.find("key=");
        if (kpos != std::string::npos) {
            key = urlDecode(body.substr(kpos + 4));
            bool deleted = dataStore_.del(key);
            std::string resp = std::string("{\"deleted\":") + (deleted ? "true" : "false") + "}";
            std::ostringstream res;
            res << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
                << resp.size() << "\r\n\r\n" << resp;
            auto s = res.str();
            sendAll(clientSocket, s.c_str(), s.size());
            closeSocket(clientSocket);
            return;
        }
    }

    // Serve static files
    if (path == "/") path = "/index.html";
    std::string full = webRoot_ + path;
    std::string data = readFileToString(full);
    if (!data.empty()) {
        std::ostringstream res;
        std::string mime = getMimeType(full);
        res << "HTTP/1.1 200 OK\r\nContent-Type: " << mime << "\r\nContent-Length: " << data.size()
            << "\r\n\r\n";
        auto head = res.str();
        sendAll(clientSocket, head.c_str(), head.size());
        sendAll(clientSocket, data.c_str(), data.size());
    } else {
        const char *notFound = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        sendAll(clientSocket, notFound, strlen(notFound));
    }
    closeSocket(clientSocket);
}

void HttpServer::closeSocket(http_socket_t s) {
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}

bool HttpServer::sendAll(http_socket_t s, const char *data, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        int n = send(s, data + sent, (int)(len - sent), 0);
        if (n <= 0) return false;
        sent += (size_t)n;
    }
    return true;
}

std::string HttpServer::urlDecode(const std::string &src) {
    std::string out;
    for (size_t i = 0; i < src.size(); ++i) {
        if (src[i] == '+') out.push_back(' ');
        else if (src[i] == '%' && i + 2 < src.size()) {
            std::string hex = src.substr(i + 1, 2);
            char c = (char)strtol(hex.c_str(), nullptr, 16);
            out.push_back(c);
            i += 2;
        } else out.push_back(src[i]);
    }
    return out;
}

std::string HttpServer::getHeaderValue(const std::string &headers, const std::string &key) {
    auto pos = headers.find(key);
    if (pos == std::string::npos) return "";
    auto end = headers.find("\r\n", pos);
    auto line = headers.substr(pos, end - pos);
    auto colon = line.find(":");
    if (colon == std::string::npos) return "";
    return trim(line.substr(colon + 1));
}

std::string HttpServer::getMimeType(const std::string &path) {
    if (path.rfind(".html") != std::string::npos) return "text/html; charset=utf-8";
    if (path.rfind(".css") != std::string::npos) return "text/css; charset=utf-8";
    if (path.rfind(".js") != std::string::npos) return "application/javascript";
    if (path.rfind(".png") != std::string::npos) return "image/png";
    if (path.rfind(".svg") != std::string::npos) return "image/svg+xml";
    return "text/plain; charset=utf-8";
}

std::string HttpServer::readFileToString(const std::string &path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) return {};
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}


