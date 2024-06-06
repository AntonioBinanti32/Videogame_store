#ifndef VIDEOGAME_STORE_SOCKETTCP_H
#define VIDEOGAME_STORE_SOCKETTCP_H

//#define WIN32_LEAN_AND_MEAN
#define NOMINMAX //Importante per evitare conflitti con mongo-cxx
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <restbed>
#include "../database/MongoDB.h"
#include "../exceptions/SocketException.h"

class SocketTcp {
public:
    SocketTcp();
    ~SocketTcp();

    bool initializeServer(const char* port);
    bool initializeClient(const char* address, const char* port);
    bool createSocket();
    void connect_to_mongodb(const std::string& hostname, const std::string& port, const std::string& database, const std::string& username, const std::string& password);
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
