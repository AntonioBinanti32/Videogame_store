#include "Handler.h"
#include <iostream>
#include <sstream>

namespace handler {

    void handleLogin(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        // Parsare il messaggio di login per ottenere username e password
        std::istringstream iss(message);
        std::string username, password;
        if (std::getline(iss, username, '/') && std::getline(iss, password, '/')) {
            std::cout << "Handling login for username: " << username << ", password: " << password << std::endl;
            const char* response = "Login successful \n";
            if (!serverSocket.sendMessage(response, clientSocket)) {
                std::cerr << "Failed to send response." << std::endl;
            }
        }
        else {
            const char* response = "Invalid login format";
            send(clientSocket, response, static_cast<int>(strlen(response)), 0);
        }
    }

    void handleSignup(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        // Parsare il messaggio di registrazione per ottenere username e password
        std::istringstream iss(message);
        std::string username, password;
        if (std::getline(iss, username, '/') && std::getline(iss, password, '/')) {
            std::cout << "Handling signup for username: " << username << ", password: " << password << std::endl;
            const char* response = "Signup successful \n";
            if (!serverSocket.sendMessage(response, clientSocket)) {
                std::cerr << "Failed to send response." << std::endl;
            }
        }
        else {
            const char* response = "Invalid signup format";
            send(clientSocket, response, static_cast<int>(strlen(response)), 0);
        }
    }

    void handleClient(SocketTcp& serverSocket, SOCKET clientSocket) {
        char recvbuf[512];
        int recvbuflen = sizeof(recvbuf);

        if (clientSocket != INVALID_SOCKET) {
            std::cout << "Client connected." << std::endl;
            bool clientConnected = true;
            while (clientConnected) {
                if (serverSocket.receiveMessage(recvbuf, recvbuflen)) {
                    std::string message(recvbuf);
                    std::cout << "Received: " << message << std::endl;

                    // Controllo messaggi
                    if (message.rfind("login/", 0) == 0) {
                        handleLogin(message.substr(6), serverSocket, clientSocket);
                    }
                    else if (message.rfind("signup/", 0) == 0) {
                        handleSignup(message.substr(7), serverSocket, clientSocket);
                    }
                    else if (message == "exit") {
                        const char* response = "Goodbye!";
                        if (!serverSocket.sendMessage(response, clientSocket)) {
                            std::cerr << "Failed to send response." << std::endl;
                        }
                        clientConnected = false;
                    }
                    else {
                        const char* response = "Unknown command";
                        if (!serverSocket.sendMessage(response, clientSocket)) {
                            std::cerr << "Failed to send response." << std::endl;
                        }
                    }
                    memset(recvbuf, 0, recvbuflen);
                }
                else {
                    // Gestisci la disconnessione del client
                    std::cerr << "Client disconnected." << std::endl;
                    clientConnected = false;
                }
            }
            // Chiudi la connessione e pulisci le risorse
            closesocket(clientSocket);
        }
        else {
            throw new HandlerException("Failed to accept client connection.");
        }
    }

}
