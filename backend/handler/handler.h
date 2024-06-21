#ifndef VIDEOGAME_STORE_HANDLER_H
#define VIDEOGAME_STORE_HANDLER_H

#include "../socketTcp/SocketTcp.h"
#include "../exceptions/HandlerException.h"
#include "../database/MongoDB.h"
#include <string>
#include "jwt/jwt.hpp"
#include <boost/url.hpp>

namespace handler {
    void handleClient(SocketTcp& serverSocket, SOCKET clientSocket);

    void handleLogin(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleSignup(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetUser(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleAddGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetGames(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetGameByTitle(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetGameByGenre(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetReviewByUser(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetReviewByGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleAddReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleAddReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleAddPurchase(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetAllPurchases(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleGetPurchases(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    //TODO: void handleGetRecommendations(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleUpdateUser(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleUpdateGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleUpdateReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleUpdatePurchase(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleUpdateReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    //TODO: void handleUpdateRecommendation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleDeleteUser(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleDeleteGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleDeleteReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleDeletePurchase(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    void handleDeleteReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
    //TODO: void handleDeleteRecommendation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket)
    bool isGameAvailableForPurchase(const std::string& game_title, int num_copies_to_purchase);
    bool removeReservationsForGame(const std::string& username, const std::string& game_title, int num_copies);
    std::string generateJwtToken(const std::string& username);
    std::string generateJson(const std::string& token, const std::string& message, bool isError = false, const std::string& user = "");
    //std::string generateJson(const std::string& token, const std::string& message, bool isError = false);
    bool verifyToken(const std::string& token, const std::string& username);
    string urlDecode(const string& SRC);
};

#endif // VIDEOGAME_STORE_HANDLER_H
