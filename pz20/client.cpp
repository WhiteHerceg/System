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

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        return 1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    
#ifdef _WIN32
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#else
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported\n";
        return 1;
    }
#endif

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cerr << "Connection to server failed.\n";
        return 1;
    }

    std::cout << "[Client] Successfully connected to server!\n";

    int numbers[2];
    std::cout << "Enter first number: ";
    if (!(std::cin >> numbers[0])) {
        std::cerr << "Invalid input. Please enter a valid integer.\n";
        return 1;
    }
    
    std::cout << "Enter second number: ";
    if (!(std::cin >> numbers[1])) {
        std::cerr << "Invalid input. Please enter a valid integer.\n";
        return 1;
    }

    send(sock, (char*)numbers, sizeof(numbers), 0);

    int sum;
    int bytesRead = recv(sock, (char*)&sum, sizeof(sum), 0);
    
    if (bytesRead == sizeof(sum)) {
        std::cout << "[Client] Server returned sum: " << sum << "\n";
    } else {
        std::cerr << "[Client] Error receiving sum from server.\n";
    }

    closesocket(sock);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
