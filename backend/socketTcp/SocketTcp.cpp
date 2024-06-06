#include "SocketTcp.h"

SocketTcp::SocketTcp() : result(nullptr), Socket(INVALID_SOCKET) {
    ZeroMemory(&hints, sizeof(hints));
}

SocketTcp::~SocketTcp() {
    cleanup();
}

bool SocketTcp::initializeServer(const char* port) {
    // Inizializzazione di Winsock versione 2.2
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        throw new SocketException("WSAStartup failed with error: " + iResult );
        return false;
    }
    return setupHints(true, nullptr, port) && createSocket();
}

bool SocketTcp::initializeClient(const char* address, const char* port) {
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        throw new SocketException("WSAStartup failed with error: " + iResult);
        return false;
    }
    return setupHints(false, address, port) && createSocket();
}

bool SocketTcp::setupHints(bool isServer, const char* address, const char* port) {
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // Indirizzo IPv4
    hints.ai_socktype = SOCK_STREAM; // Specifica stream socket
    hints.ai_protocol = IPPROTO_TCP; // Specifica l'utilizzo del protocollo TCP

    if (isServer) {
        hints.ai_flags = AI_PASSIVE; // Server mode: socket di ascolto
    }

    int iResult = getaddrinfo(address, port, &hints, &result);
    if (iResult != 0) {
        throw new SocketException("getaddrinfo failed with error: " + iResult );
        WSACleanup();
        return false;
    }
    return true;
}

bool SocketTcp::createSocket() {
    Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (Socket == INVALID_SOCKET) {
        throw new SocketException("Error at socket(): " + WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    if (hints.ai_flags & AI_PASSIVE) {
        // Bind server socket
        if (::bind(Socket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
            throw new SocketException("Bind failed with error: " + WSAGetLastError());
            freeaddrinfo(result);
            closesocket(Socket);
            WSACleanup();
            return false;
        }
    }

    return true;
}

void SocketTcp::connect_to_mongodb(const std::string& hostname, const std::string& port, const std::string& database, const std::string& username, const std::string& password) {
    MongoDB* mongoDb = MongoDB::getInstance();
    mongoDb->connectDB(hostname, port, database, username, password);
}

void SocketTcp::cleanup() {
    if (result != nullptr) {
        freeaddrinfo(result);
        result = nullptr;
    }
    if (Socket != INVALID_SOCKET) {
        closesocket(Socket);
        Socket = INVALID_SOCKET;
    }
    if (ClientSocket != INVALID_SOCKET) {
        closesocket(ClientSocket);
        ClientSocket = INVALID_SOCKET;
    }
    WSACleanup();
}

bool SocketTcp::listenForConnections(int backlog) {
    if (listen(Socket, backlog) == SOCKET_ERROR) {
        throw new SocketException("Listen failed with error: " + WSAGetLastError());
        cleanup();
        return false;
    }
    return true;
}

SOCKET SocketTcp::acceptConnection() {
    ClientSocket = accept(Socket, nullptr, nullptr);
    if (ClientSocket == INVALID_SOCKET) {
        throw new SocketException("Accept failed with error: " + WSAGetLastError());
        cleanup();
        return INVALID_SOCKET;
    }
    return ClientSocket;
}

bool SocketTcp::sendMessage(const char* message, SOCKET clientSocket) {
    std::cout << "Socket" << clientSocket << " message: " << message << std::endl;
    int iResult = send(clientSocket, message, static_cast<int>(strlen(message)), 0);
    if (iResult == SOCKET_ERROR) {
        throw new SocketException("Send failed with error: " + WSAGetLastError());
        cleanup();
        return false;
    }
    return true;
}

bool SocketTcp::receiveMessage(char* buffer, int bufferSize) {
    int iResult = recv(ClientSocket, buffer, bufferSize, 0);
    if (iResult > 0) {
        buffer[iResult] = '\0'; // Null termina i dati ricevuti
        return true;
    }
    else if (iResult == 0) {
        std::cout << "Connection closed" << std::endl;
    }
    else {
        throw new SocketException("Receive failed with error: " + WSAGetLastError());
    }
    return false;
}

bool SocketTcp::connectToServer() {
    // Tentativo di connessione al Server
    int iResult = connect(Socket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        throw new SocketException("Connect failed with error: " + WSAGetLastError());
        closesocket(Socket);
        Socket = INVALID_SOCKET;
        return false;
    }
    return true;
}

