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
        //TODO: throw new ScketException("WSAStartup failed with error: " << iResult << std::endl")
        std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
        return false;
    }
    return setupHints(true, nullptr, port) && createSocket();
}

bool SocketTcp::initializeClient(const char* address, const char* port) {
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        //TODO: throw new ScketException("WSAStartup failed with error: " << iResult << std::endl")
        std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
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
        //TODO: throw new ScketException(""getaddrinfo failed with error: " << iResult << std::endl")
        std::cerr << "getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return false;
    }
    return true;
}

bool SocketTcp::createSocket() {
    Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (Socket == INVALID_SOCKET) {
        //TODO: throw new ScketException(""Error at socket(): " << WSAGetLastError() << std::endl")
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    if (hints.ai_flags & AI_PASSIVE) {
        // Bind server socket
        if (bind(Socket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
            //TODO: throw new ScketException(""Bind failed with error: " << WSAGetLastError() << std::endl")
            std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
            freeaddrinfo(result);
            closesocket(Socket);
            WSACleanup();
            return false;
        }
    }

    return true;
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
        //TODO: throw new ScketException(""Listen failed with error: " << WSAGetLastError() << std::endl")
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        cleanup();
        return false;
    }
    return true;
}

SOCKET SocketTcp::acceptConnection() {
    ClientSocket = accept(Socket, nullptr, nullptr);
    if (ClientSocket == INVALID_SOCKET) {
        //TODO: throw new ScketException(""Accept failed with error: " << WSAGetLastError() << std::endl")
        std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
        cleanup();
        return INVALID_SOCKET;
    }
    return ClientSocket;
}

bool SocketTcp::sendMessage(const char* message, SOCKET clientSocket) {
    std::cout << "Socket" << clientSocket << " message: " << message << std::endl;
    int iResult = send(clientSocket, message, static_cast<int>(strlen(message)), 0);
    if (iResult == SOCKET_ERROR) {
        //TODO: throw new ScketException(""Send failed with error: " << WSAGetLastError() << std::endl")
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
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
        //TODO: throw new ScketException(""Receive failed with error: " << WSAGetLastError() << std::endl")
        std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
    }
    return false;
}

bool SocketTcp::connectToServer() {
    // Tentativo di connessione al Server
    int iResult = connect(Socket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        //TODO: throw new ScketException(""Connect failed with error: " << WSAGetLastError() << std::endl")
        std::cerr << "Connect failed with error: " << WSAGetLastError() << std::endl;
        closesocket(Socket);
        Socket = INVALID_SOCKET;
        return false;
    }
    return true;
}

