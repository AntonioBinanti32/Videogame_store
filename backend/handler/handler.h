#ifndef VIDEOGAME_STORE_HANDLER_H
#define VIDEOGAME_STORE_HANDLER_H

#include "../socketTcp/SocketTcp.h"
#include "../exceptions/HandlerException.h"
#include "../database/MongoDB.h"
#include <string>

namespace handler {
    void handleClient(SocketTcp& serverSocket, SOCKET clientSocket);

    void handleLogin(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleSignup(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleAddGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetGames(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleAddReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleAddReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetRecommendations(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);

    //TODO: Implementare metodi update
};

#endif // VIDEOGAME_STORE_HANDLER_H
