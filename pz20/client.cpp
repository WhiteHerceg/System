/**
 * @file network_client.cpp
 * @brief Client for sum calculation service
 * @version 1.2.0
 */

#include <iostream>
#include <limits>

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

// Simple RAII wrapper
class SocketInitializer {
public:
    SocketInitializer() : ok_(true) {
#ifdef _WIN32
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            std::cerr << "[Client] Failed to initialize networking\n";
            ok_ = false;
        }
#endif
    }
    
    ~SocketInitializer() {
#ifdef _WIN32
        WSACleanup();
#endif
    }
    
    bool isOk() const { return ok_; }
    
private:
    bool ok_;
};

int readNumber(const std::string& prompt) {
    int value;
    std::cout << prompt;
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter an integer: ";
    }
    return value;
}

int main() {
    SocketInitializer netInit;
    if (!netInit.isOk()) return 1;
    
    // Create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "[Client] Could not create socket\n";
        return 1;
    }
    
    // Configure server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    
#ifdef _WIN32
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#else
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        std::cerr << "[Client] Invalid server address\n";
        closesocket(sock);
        return 1;
    }
#endif
    
    // Connect
    std::cout << "[Client] Connecting to server at 127.0.0.1:8080...\n";
    
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "[Client] Connection failed. Is the server running?\n";
        closesocket(sock);
        return 1;
    }
    
    std::cout << "[Client] Connected successfully!\n";
    
    // Get input
    int numbers[2];
    numbers[0] = readNumber("Enter first number: ");
    numbers[1] = readNumber("Enter second number: ");
    
    // Send data
    if (send(sock, (char*)numbers, sizeof(numbers), 0) != sizeof(numbers)) {
        std::cerr << "[Client] Failed to send data\n";
        closesocket(sock);
        return 1;
    }
    
    std::cout << "[Client] Sent: " << numbers[0] << ", " << numbers[1] << "\n";
    
    // Receive result
    int result;
    int bytes = recv(sock, (char*)&result, sizeof(result), 0);
    
    if (bytes == sizeof(result)) {
        std::cout << "[Client] Server response: " << numbers[0] 
                  << " + " << numbers[1] << " = " << result << "\n";
    } else {
        std::cerr << "[Client] Failed to receive result (got " 
                  << bytes << " bytes)\n";
    }
    
    closesocket(sock);
    std::cout << "[Client] Disconnected\n";
    
    return 0;
}
