#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // Создание сокета
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(1);
    }

    // Связывание сокета с адресом
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    // Прослушивание сокета
    if (listen(server_socket, 5) == -1) {
        perror("listen");
        exit(1);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        // Принятие нового соединения
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("accept");
            continue;
        }

        std::cout << "Client connected from " << inet_ntoa(client_addr.sin_addr) << std::endl;

        // Цикл приема-передачи данных
        while (true) {
            char buffer[BUFFER_SIZE];
            int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (bytes_received == -1) {
                perror("recv");
                break;
            } else if (bytes_received == 0) {
                std::cout << "Client disconnected" << std::endl;
                break;
            }

            std::cout << "Received from client: " << std::string(buffer, bytes_received) << std::endl;

            // Отправка подтверждения о получении
            std::string response = "Message received";
            int bytes_sent = send(client_socket, response.data(), response.size(), 0);
            if (bytes_sent == -1) {
                perror("send");
                break;
            }

            std::cout << "Sent to client: " << response << std::endl;
        }

        close(client_socket);
    }

    close(server_socket);

    return 0;
}
