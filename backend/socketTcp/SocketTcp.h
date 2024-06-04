#ifndef VIDEOGAME_STORE_SOCKETTCP_H
#define VIDEOGAME_STORE_SOCKETTCP_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

class SocketTcp {
public:
    SocketTcp();
    ~SocketTcp();

    bool initializeServer(const char* port);
    bool initializeClient(const char* address, const char* port);
    bool createSocket();
    void cleanup();

    bool listenForConnections(int backlog = SOMAXCONN);
    SOCKET acceptConnection();
    bool sendMessage(const char* message, SOCKET clientSocket);
    bool receiveMessage(char* buffer, int bufferSize);
    bool connectToServer();

private:
    bool setupHints(bool isServer, const char* address, const char* port);

    WSADATA wsaData;
    struct addrinfo hints, * result;
    SOCKET Socket;
    SOCKET ClientSocket;
};

#endif // VIDEOGAME_STORE_SOCKETTCP_H
