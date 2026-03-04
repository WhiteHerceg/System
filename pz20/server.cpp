#include <iostream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
    typedef int SOCKET;
#endif

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }
#endif

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        return 1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed.\n";
        return 1;
    }

    if (listen(server_fd, 3) == SOCKET_ERROR) {
        std::cerr << "Listen failed.\n";
        return 1;
    }

    std::cout << "[Server] Waiting for connection on port 8080...\n";

    SOCKET new_socket;
    struct sockaddr_in client_address;
#ifdef _WIN32
    int addrlen = sizeof(client_address);
#else
    socklen_t addrlen = sizeof(client_address);
#endif

    new_socket = accept(server_fd, (struct sockaddr *)&client_address, &addrlen);
    if (new_socket == INVALID_SOCKET) {
        std::cerr << "Accept failed.\n";
        return 1;
    }

    std::cout << "[Server] Client connected successfully!\n";

    int numbers[2];
    int bytesRead = recv(new_socket, (char*)numbers, sizeof(numbers), 0);
    
    if (bytesRead == sizeof(numbers)) {
        std::cout << "[Server] Received numbers from client: " << numbers[0] << " and " << numbers[1] << "\n";
        
        int sum = numbers[0] + numbers[1];
        send(new_socket, (char*)&sum, sizeof(sum), 0);
        std::cout << "[Server] Sent sum: " << sum << "\n";
    } else {
        std::cerr << "[Server] Error receiving data.\n";
    }

    closesocket(new_socket);
    closesocket(server_fd);

#ifdef _WIN32
    WSACleanup();
#endif
    
    std::cout << "[Server] Server shut down.\n";
    return 0;
}
