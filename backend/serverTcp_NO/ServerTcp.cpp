/*#include "ServerTcp.h"
#include <iostream>
#include <cstring>

SocketTCPServer::SocketTCPServer(int port) {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        throw SocketException("WSAStartup failed");
    }

    server_sockid = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockid == INVALID_SOCKET) {
        WSACleanup();
        throw SocketException("Server socket creation failed");
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(server_sockid, (struct sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        closesocket(server_sockid);
        WSACleanup();
        throw SocketException("Binding failed");
    }

    if (listen(server_sockid, 5) == SOCKET_ERROR) {
        closesocket(server_sockid);
        WSACleanup();
        throw SocketException("Listening failed");
    }

    std::cout << "Server is listening on port " << port << std::endl;
}

int SocketTCPServer::acceptConnection(struct sockaddr_in& client_address) {
    int client_len = sizeof(client_address);
    SOCKET client_sockid = accept(server_sockid, (struct sockaddr*)&client_address, &client_len);
    if (client_sockid == INVALID_SOCKET) {
        throw SocketException("Accepting connection failed");
    }
    return client_sockid;
}

SocketTCPServer::~SocketTCPServer() {
    closesocket(server_sockid);
    WSACleanup();
    std::cout << "Server socket closed" << std::endl;
}*/
