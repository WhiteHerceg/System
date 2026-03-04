/**
 * @file network_server.cpp
 * @brief Simple TCP server that receives two integers and returns their sum
 * @version 1.2.0
 */

#include <iostream>
#include <memory>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
    typedef int SOCKET;
#endif

// RAII wrapper for Windows sockets
class WinSockGuard {
public:
    WinSockGuard() : initialized_(false) {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
            initialized_ = true;
            std::cout << "[Server] Windows sockets initialized\n";
        } else {
            std::cerr << "[Server] Failed to initialize Windows sockets\n";
        }
#endif
    }
    
    ~WinSockGuard() {
#ifdef _WIN32
        if (initialized_) {
            WSACleanup();
            std::cout << "[Server] Windows sockets cleaned up\n";
        }
#endif
    }
    
    bool good() const { return initialized_; }
    
private:
    bool initialized_;
};

class TCPServer {
public:
    TCPServer(int port) : server_fd_(INVALID_SOCKET), port_(port) {}
    
    ~TCPServer() {
        if (server_fd_ != INVALID_SOCKET) {
            closesocket(server_fd_);
            std::cout << "[Server] Socket closed\n";
        }
    }
    
    bool initialize() {
        // Create socket
        server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd_ == INVALID_SOCKET) {
            std::cerr << "[Server] Socket creation failed\n";
            return false;
        }
        
        // Configure address
        address_.sin_family = AF_INET;
        address_.sin_addr.s_addr = INADDR_ANY;
        address_.sin_port = htons(port_);
        
        // Bind
        if (bind(server_fd_, (struct sockaddr*)&address_, sizeof(address_)) == SOCKET_ERROR) {
            std::cerr << "[Server] Bind failed on port " << port_ << "\n";
            return false;
        }
        
        // Listen
        if (listen(server_fd_, 5) == SOCKET_ERROR) {
            std::cerr << "[Server] Listen failed\n";
            return false;
        }
        
        std::cout << "[Server] Listening on port " << port_ << "\n";
        return true;
    }
    
    bool acceptClient() {
        socklen_t addr_len = sizeof(client_address_);
        client_fd_ = accept(server_fd_, (struct sockaddr*)&client_address_, &addr_len);
        
        if (client_fd_ == INVALID_SOCKET) {
            std::cerr << "[Server] Accept failed\n";
            return false;
        }
        
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_address_.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "[Server] Client connected from " << client_ip << "\n";
        
        return true;
    }
    
    void processClient() {
        int numbers[2];
        int bytes_received = recv(client_fd_, (char*)numbers, sizeof(numbers), 0);
        
        if (bytes_received == sizeof(numbers)) {
            std::cout << "[Server] Received: " << numbers[0] << ", " << numbers[1] << "\n";
            
            int result = numbers[0] + numbers[1];
            send(client_fd_, (char*)&result, sizeof(result), 0);
            
            std::cout << "[Server] Sent result: " << result << "\n";
        } else {
            std::cerr << "[Server] Incomplete data received (" 
                      << bytes_received << " bytes)\n";
        }
        
        closesocket(client_fd_);
        client_fd_ = INVALID_SOCKET;
        std::cout << "[Server] Client disconnected\n";
    }
    
private:
    SOCKET server_fd_;
    SOCKET client_fd_ {INVALID_SOCKET};
    struct sockaddr_in address_;
    struct sockaddr_in client_address_;
    int port_;
};

int main() {
    WinSockGuard winsock;
#ifndef _WIN32
    // On non-Windows, we consider winsock always "good"
#else
    if (!winsock.good()) return 1;
#endif
    
    TCPServer server(8080);
    
    if (!server.initialize()) {
        return 1;
    }
    
    std::cout << "[Server] Waiting for client...\n";
    
    if (server.acceptClient()) {
        server.processClient();
    }
    
    std::cout << "[Server] Shutting down\n";
    return 0;
}
