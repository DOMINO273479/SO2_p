#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#define PORT 8080
#define BUFFER_SIZE 1024

struct Client {
    SOCKET socket;
    std::string name;
};

std::vector<Client> clients;
std::mutex clients_mutex;

void broadcast(const std::string& message, SOCKET sender_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (const auto& client : clients) {
        if (client.socket != sender_socket) {
            send(client.socket, message.c_str(), message.length(), 0);
        }
    }
}

void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    std::string client_name;

    int bytes = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes <= 0) {
        closesocket(client_socket);
        return;
    }

    buffer[bytes] = '\0';
    client_name = buffer;

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back({ client_socket, client_name });
    }

    std::string welcome = ">> " + client_name + " joined the chat.\n";
    std::cout << welcome;
    broadcast(welcome, client_socket);

    while (true) {
        bytes = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes <= 0) {
            break;
        }

        buffer[bytes] = '\0';
        std::string message = client_name + ": " + buffer + "\n";
        std::cout << message;
        broadcast(message, client_socket);
    }

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (auto it = clients.begin(); it != clients.end(); ++it) {
            if (it->socket == client_socket) {
                clients.erase(it);
                break;
            }
        }
    }

    std::string bye = ">> " + client_name + " left the chat.\n";
    std::cout << bye;
    broadcast(bye, client_socket);
    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed.\n";
        return 1;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed.\n";
        return 1;
    }

    std::cout << "Chat server started on port " << PORT << "\n";

    while (true) {
        sockaddr_in client_addr;
        int client_size = sizeof(client_addr);
        SOCKET client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_size);

        if (client_socket != INVALID_SOCKET) {
            std::thread(handle_client, client_socket).detach();
        }
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
