#include "Handler.h"
#include <iostream>
#include <sstream>
#include <boost/url.hpp>

// TODO: Implementare Boost-url
// TODO: Implemendare sistema di raccomandazioni
// TODO: Inserire admin nella verifica token delle funzioni di gestione importanti

namespace handler {

    void handleLogin(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, password;
        if (std::getline(iss, username, '*') && std::getline(iss, password, '*')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->login(username, password);
                std::string token = generateJwtToken(username);
                //Incapsulamento in un json
                std::string response = generateJson(token, "Login successfully", false, username);
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const LoginException& e) {
                std::string error = generateJson("", "Login failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Login failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string error = generateJson("", "Invalid format", true);
            serverSocket.sendMessage(error.c_str(), clientSocket);
            return;
        }
    }

    void handleSignup(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, password, imageUrl;
        if (std::getline(iss, username, '*') && std::getline(iss, password, '*') && std::getline(iss, imageUrl, '*')) {
            try {
                if (imageUrl.empty()) {
                    imageUrl = "https://www.iconpacks.net/icons/2/free-user-icon-3297-thumb.png";
                }
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->signup(username, password,imageUrl);
                std::string token = generateJwtToken(username);
                //Incapsulamento in un json
                std::string response = generateJson(token, "Signup successfully", false, username);
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const SignupException& e) {
                std::string error = generateJson("", "Signup failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Signup failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string error = generateJson("", "Invalid signup format", true);
            serverSocket.sendMessage(error.c_str(), clientSocket);
            return;
        }
    }

    void handleGetUser(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, actualUser, token;
        if (std::getline(iss, username, '*') && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    nlohmann::json user = mongoDb->getUser(username);
                    std::string response = generateJson("", user.dump(4)); // Converte il JSON in una stringa formattata con indentazione
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const UserNotFoundException& e) {
                std::string error = generateJson("", "Get user failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Get user failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Get user failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid get user format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleAddGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string title, genre, release_date, developer, price_str, stock_str, description, imageUrl, actualUser, token;
        if (std::getline(iss, title, '*') && std::getline(iss, genre, '*') && std::getline(iss, release_date, '*') &&
            std::getline(iss, developer, '*') && std::getline(iss, price_str, '*') && std::getline(iss, stock_str, '*') &&
            std::getline(iss, description, '*') && std::getline(iss, imageUrl, '*') 
            && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {

            try {
                double price = std::stod(price_str);
                int stock = std::stoi(stock_str);
                if (imageUrl.empty()) {
                    imageUrl = "https://www.creativefabrica.com/wp-content/uploads/2023/05/08/Video-Game-Controller-Logo-Graphics-69127373-1-580x387.png";
                }
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    mongoDb->addGame(title, genre, release_date, developer, price, stock, description, imageUrl);
                    std::string response = generateJson("", "Game added successfully");
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const CreateGameException& e) {
                std::string error = generateJson("", "Add game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Add game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Add game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid add game format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }
    
    void handleGetGames(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string actualUser, token;
        if (std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    nlohmann::json games = mongoDb->getGames();
                    std::string response = generateJson("", games.dump(4)); // Converte il JSON in una stringa formattata con indentazione
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const GetGameException& e) {
                std::string error = generateJson("", "Get games failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Get games failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Get games failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid get games format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleGetGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string game_id, actualUser, token;
        if (std::getline(iss, game_id, '*') && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    nlohmann::json game = mongoDb->getGame(game_id);
                    std::string response = generateJson("", game.dump(4)); // Converte il JSON in una stringa formattata con indentazione
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const GetGameException& e) {
                std::string error = generateJson("", "Get game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Get game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Get game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid get game format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleGetGameByTitle(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string title, actualUser, token;
        if (std::getline(iss, title, '*') && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    nlohmann::json games = mongoDb->getGameByTitle(title);
                    std::string response = generateJson("", games.dump(4)); // Converte il JSON in una stringa formattata con indentazione
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const GetGameException& e) {
                std::string error = generateJson("", "Get game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Get game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Get game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid get game format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleGetGameByGenre(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string genre, actualUser, token;
        if (std::getline(iss, genre, '*') && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    nlohmann::json games = mongoDb->getGameByGenre(genre);
                    std::string response = generateJson("", games.dump(4)); // Converte il JSON in una stringa formattata con indentazione
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const GetGameException& e) {
                std::string error = generateJson("", "Get game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Get game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Get game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid get game format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleGetReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string review_id, actualUser, token;
        if (std::getline(iss, review_id, '*') && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    nlohmann::json review = mongoDb->getReview(review_id);
                    std::string response = generateJson("", review.dump(4)); // Converte il JSON in una stringa formattata con indentazione
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const ReviewException& e) {
                std::string error = generateJson("", "Get review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Get review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Get review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid get review format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleGetReviewByUser(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string user, actualUser, token;
        if (std::getline(iss, user, '*') && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    nlohmann::json review = mongoDb->getReviewByUser(user);
                    std::string response = generateJson("", review.dump(4)); // Converte il JSON in una stringa formattata con indentazione
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const ReviewException& e) {
                std::string error = generateJson("", "Get review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const UserNotFoundException& e) {
                std::string error = generateJson("", "Get review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Get review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Get review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid get review format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleGetReviewByGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string game_title, actualUser, token;
        if (std::getline(iss, game_title, '*') && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    nlohmann::json review = mongoDb->getReviewByGame(game_title);
                    std::string response = generateJson("", review.dump(4)); // Converte il JSON in una stringa formattata con indentazione
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const ReviewException& e) {
                std::string error = generateJson("", "Get review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const GetGameException& e) {
                std::string error = generateJson("", "Get review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Get review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Get review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid get review format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleAddReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, review_text, rating_str, actualUser, token;
        if (std::getline(iss, username, '*') && std::getline(iss, game_title, '*') &&
            std::getline(iss, review_text, '*') && std::getline(iss, rating_str, '*') &&
            std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {

            try {
                int rating = std::stoi(rating_str);
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    mongoDb->addReview(username, game_title, review_text, rating);
                    std::string response = generateJson("", "Review added successfully");
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const UserNotFoundException& e) {
                std::string error = generateJson("", "Add review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const GetGameException& e) {
                std::string error = generateJson("", "Add review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const CreateReviewException& e) {
                std::string error = generateJson("", "Add review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Add review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Add review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid add review format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleAddReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, num_copies_str, actualUser, token;
        if (std::getline(iss, username, '*') && std::getline(iss, game_title, '*') && std::getline(iss, num_copies_str, '*') &&
            std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                int num_copies = std::stoi(num_copies_str);
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    mongoDb->addReservation(username, game_title, num_copies);
                    std::string response = generateJson("", "Reservation added successfully");
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const UserNotFoundException& e) {
                std::string error = generateJson("", "Add reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const GetGameException& e) {
                std::string error = generateJson("", "Add reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const CreateReservationException& e) {
                std::string error = generateJson("", "Add reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Add reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Add reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid add reservation format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    //

    void handleAddPurchase(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, num_copies_str, actualUser, token;
        if (std::getline(iss, username, '*') && std::getline(iss, game_title, '*') && std::getline(iss, num_copies_str, '*') &&
            std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                int num_copies = std::stoi(num_copies_str);
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    // Verifica se il gioco è disponibile per l'acquisto
                    if (!isGameAvailableForPurchase(game_title, num_copies)) {
                        throw PurchaseException("Not enough copies available for purchase");
                    }
                    // Effettua l'acquisto
                    mongoDb->addPurchase(username, game_title, num_copies);
                    // Rimuove le prenotazioni se l'utente ha già prenotato questo gioco
                    removeReservationsForGame(username, game_title, num_copies);
                    std::string response = generateJson("", "Purchase added successfully");
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const UserNotFoundException& e) {
                std::string error = generateJson("", "Add purchase failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const GetGameException& e) {
                std::string error = generateJson("", "Add purchase failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const PurchaseException& e) {
                std::string error = generateJson("", "Add purchase failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Add purchase failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Add purchase failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid add purchase format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleGetReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, actualUser, token;
        if (std::getline(iss, username, '*') && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    nlohmann::json reservations = mongoDb->getReservations(username);
                    std::string response = generateJson("", reservations.dump(4)); // Converte il JSON in una stringa formattata con indentazione
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const UserNotFoundException& e) {
                std::string error = generateJson("", "Get reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const ReservationException& e) {
                std::string error = generateJson("", "Get reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Get reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Get reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid get reservation format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleGetAllPurchases(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string actualUser, token;
        if (std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    nlohmann::json purchases = mongoDb->getAllPurchases();
                    std::string response = generateJson("", purchases.dump(4)); // Converte il JSON in una stringa formattata con indentazione
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const UserNotFoundException& e) {
                std::string error = generateJson("", "Get purchases failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const PurchaseException& e) {
                std::string error = generateJson("", "Get purchases failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Get purchases failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Get purchases failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid get purchases format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleGetPurchases(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, actualUser, token;
        if (std::getline(iss, username, '*') && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    nlohmann::json purchases = mongoDb->getPurchases(username);
                    std::string response = generateJson("", purchases.dump(4)); // Converte il JSON in una stringa formattata con indentazione
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const UserNotFoundException& e) {
                std::string error = generateJson("", "Get purchases failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const PurchaseException& e) {
                std::string error = generateJson("", "Get purchases failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Get purchases failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Get purchases failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid get purchases format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }
    

    //TODO: Implementare handleGetRecommendations
    /*
    void handleGetRecommendations(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username;
        if (std::getline(iss, username, '*')) {
            try {
                //std::vector<bsoncxx::document::value> recommendations = getRecommendations(username);
                std::string response = "Recommendations:";  // Convert recommendations to string
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const std::exception& e) {
                std::string error = "Get recommendations failed: " + std::string(e.what(), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid get recommendations format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }*/

    void handleUpdateUser(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, password, imageUrl, actualUser, token;
        if (std::getline(iss, username, '*') && std::getline(iss, password, '*') && std::getline(iss, imageUrl, '*') &&
            std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    mongoDb->updateUser(username, password, imageUrl);
                    std::string response = generateJson("", "User modified successful");
                    // TODO: Return JWT
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const UserNotFoundException& e) {
                std::string error = generateJson("", "Modify user failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const HandlerException& e) {
                std::string error = generateJson("", "Modify user failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Modify user failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Modify user failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid Modify user format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleUpdateGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string title, genre, release_date, developer, price_str, stock_str, description, imageUrl, actualUser, token;
        if (std::getline(iss, title, '*') && std::getline(iss, genre, '*') && std::getline(iss, release_date, '*') &&
            std::getline(iss, developer, '*') && std::getline(iss, price_str, '*') && std::getline(iss, stock_str, '*') &&
            std::getline(iss, description, '*') && std::getline(iss, imageUrl, '*') &&
            std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {

            try {
                double price = std::stod(price_str);
                int stock = std::stoi(stock_str);
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    mongoDb->updateGame(title, genre, release_date, developer, price, stock, description, imageUrl);
                    std::string response = generateJson("", "Game updated successfully");
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const GetGameException& e) {
                std::string error = generateJson("", "Update game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const HandlerException& e) {
                std::string error = generateJson("", "Update game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Update game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Update game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid update game format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }


    void handleUpdateReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, newNumCopies_str, actualUser, token;
        if (std::getline(iss, username, '*') && std::getline(iss, game_title, '*') && std::getline(iss, newNumCopies_str, '*') &&
            std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    int newNumCopies = std::stoi(newNumCopies_str);
                    MongoDB* mongoDb = MongoDB::getInstance();
                    mongoDb->updateReservation(username, game_title, newNumCopies);
                    std::string response = generateJson("", "Reservation updated successfully");
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const ReservationException& e) {
                std::string error = generateJson("", "Update reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const mongocxx::exception& e) {
                std::string error = generateJson("", "Update reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Update reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Update reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid update reservation format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleUpdatePurchase(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, newNumCopies_str, purchase_id, actualUser, token;
        if (std::getline(iss, newNumCopies_str, '*') &&
            std::getline(iss, purchase_id, '*') && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    int newNumCopies = std::stoi(newNumCopies_str);
                    // Verifica se il gioco è disponibile per l'acquisto
                    // Altrimenti, solleva un'eccezione PurchaseException
                    if (!isGameAvailableForPurchase(game_title, newNumCopies)) {
                        throw PurchaseException("Not enough copies available for purchase");
                    }
                    MongoDB* mongoDb = MongoDB::getInstance();
                    // Aggiorna l'acquisto
                    mongoDb->updatePurchase(newNumCopies, purchase_id);
                    std::string response = generateJson("", "Purchase updated successfully");
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const ReservationException& e) {
                std::string error = generateJson("", "Update purchase failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const mongocxx::exception& e) {
                std::string error = generateJson("", "Update purchase failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Update purchase failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Update purchase failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid update purchase format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleUpdateReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, newReviewText, newRating_str, actualUser, token;
        if (std::getline(iss, username, '*') && std::getline(iss, game_title, '*') &&
            std::getline(iss, newReviewText, '*') && std::getline(iss, newRating_str, '*') &&
            std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {

            try {
                if (verifyToken(token, actualUser)) {
                    int newRating = std::stoi(newRating_str);
                    MongoDB* mongoDb = MongoDB::getInstance();
                    mongoDb->updateReview(username, game_title, newReviewText, newRating);
                    std::string response = generateJson("", "Review updated successfully");
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const ReviewException& e) {
                std::string error = generateJson("", "Update review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const mongocxx::exception& e) {
                std::string error = generateJson("", "Update review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const jwt::TokenExpiredError& e) {
                std::string error = generateJson("", "Update review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Update review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid update review format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleDeleteUser(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, actualUser, token;
        if (std::getline(iss, username, '*') && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    mongoDb->deleteUser(username);
                    std::string response = generateJson("", "User deleted successfully");
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (const UserNotFoundException& e) {
                std::string error = generateJson("", "Delete User failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Delete User failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid delete user format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleDeleteGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string game_title, actualUser, token;
        if (std::getline(iss, game_title, '*') && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {
            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    mongoDb->deleteGame(game_title);
                    std::string response = generateJson("", "Game deleted successfully");
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (GetGameException& e) {
                std::string error = generateJson("", "Delete Game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Delete game failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid delete game format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleDeleteReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string reservationId, actualUser, token;
        if (std::getline(iss, reservationId, '*') &&
            std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {

            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    mongoDb->deleteReservation(reservationId);
                    std::string response = generateJson("", "Reservation deleted successfully");
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (GetGameException& e) {
                std::string error = generateJson("", "Delete reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Delete reservation failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid delete reservation format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    //TODO: Sistemare con solo l'id

    void handleDeletePurchase(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, purchase_id, actualUser, token;
        if (std::getline(iss, purchase_id, '*') && std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {

            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    // Elimina l'acquisto
                    mongoDb->deletePurchase(purchase_id);
                    std::string response = generateJson("", "Purchase deleted successfully");
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (GetGameException& e) {
                std::string error = generateJson("", "Delete purchase failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Delete purchase failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid delete purchase format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    void handleDeleteReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, actualUser, token;
        if (std::getline(iss, username, '*') && std::getline(iss, game_title, '*') &&
            std::getline(iss, actualUser, '*') && std::getline(iss, token, '*')) {

            try {
                if (verifyToken(token, actualUser)) {
                    MongoDB* mongoDb = MongoDB::getInstance();
                    mongoDb->deleteReview(username, game_title);
                    std::string response = generateJson("", "Review deleted successfully");
                    serverSocket.sendMessage(response.c_str(), clientSocket);
                }
                else {
                    std::string error = generateJson("", "Errore verifica del token", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    return;
                }
            }
            catch (UserNotFoundException& e) {
                std::string error = generateJson("", "Delete review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (GetGameException& e) {
                std::string error = generateJson("", "Delete review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (ReviewException& e) {
                std::string error = generateJson("", "Delete review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = generateJson("", "Delete review failed: " + std::string(e.what()), true);
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            std::string response = generateJson("", "Invalid delete review format", true);
            serverSocket.sendMessage(response.c_str(), clientSocket);
            return;
        }
    }

    //TODO: Da implementare handleDeleteRecommendation
    /*
    void handleDeleteRecommendation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title;
        if (std::getline(iss, username, '*') && std::getline(iss, game_title, '*')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->deleteReview(username, game_title);
                const char* response = "review deleted successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (UserNotFoundException& e) {
                std::string error = "Delete reservation failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (GetGameException& e) {
                std::string error = "Delete reservation failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (ReviewException& e) {
                std::string error = "Delete reservation failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Delete review failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid delete review format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }*/

    //TODO
    bool isGameAvailableForPurchase(const std::string& game_title, int num_copies_to_purchase) {
        MongoDB* mongoDb = MongoDB::getInstance();
        // Recupera il numero di copie disponibili per il gioco specificato
        nlohmann::json game = mongoDb->getGameByTitle(game_title);
        int stock = game["stock"].get<int>();
        if (stock >= num_copies_to_purchase) {
            int stock_new = stock - num_copies_to_purchase;
            mongoDb->updateGame(game_title, "", "", "", -1, stock_new, "", "");
            return true;
        }
        else {
            return false;
        }
    }

    bool removeReservationsForGame(const std::string&  username, const std::string& game_title, int num_copies_purchased) {
        int reserved = 0;
        MongoDB* mongoDb = MongoDB::getInstance();
        // Recupera il numero di copie prenotate per il gioco specificato
        nlohmann::json reservations = mongoDb->getReservations(username);
        for (const auto& reservation : reservations) {
            if (reservation["game_title"] == game_title) {
                reserved += reservation["num_copies"].get<int>();
            }
        }
        if (reserved == 0) {
            return false;
        }
        else if (reserved < num_copies_purchased) {
            mongoDb->deleteReservation(reservations["_id"]);
        }
        else {
            int reserved_new = reserved - num_copies_purchased;
            mongoDb->updateReservation(username, game_title, reserved_new);
        }
        return true;
    }

    std::string generateJwtToken(const std::string& username) {
        // Restituisco all'utente un jwt token che verrà usato per autenticare l'utente per le varie richieste
        jwt::jwt_object obj{ jwt::params::algorithm("HS256"), jwt::params::payload({{"username", username}}), jwt::params::secret("secret") };
        // Aggiunta scadenza al token
        obj.add_claim("exp", std::chrono::system_clock::now() + std::chrono::hours(2));
        return obj.signature();
    }

    std::string generateJson(const std::string& token, const std::string& message, bool isError, const std::string& user) {
        nlohmann::json response;
        response["message"] = message;
        response["error"] = isError;
        if (token != "") {
            response["token"] = token;
        }
        if (!user.empty()) {
            response["user"] = user;
        }
        
        return response.dump();
    }

    // Sovraccarico della funzione per accettare solo il token, il messaggio e isError
    /*std::string generateJson(const std::string& token, const std::string& message, bool isError) {
        return generateJson(token, message, "", isError);
    }*/

    bool verifyToken(const std::string& token, const std::string& username) {
        try {
            // Verifico che il token passato nel campo Authorization del header sia corretto; se così non fosse restituisco un errore al client
            auto dec_token = jwt::decode(token, jwt::params::algorithms({ "HS256" }), jwt::params::secret("secret"));
            if (username == dec_token.payload().get_claim_value<std::string>("username")) {
                return true;
            }
            else {
                return false;
            }
        }
        catch (const jwt::TokenExpiredError& e) {
            throw;
        }
    }

    std::string urlDecode(const std::string& SRC) {
        boost::core::string_view s = SRC;
        boost::system::result<boost::urls::url_view> r = boost::urls::parse_uri(s);
        boost::urls::url_view u = r.value();
        std::string urlString(u.data(), u.size());
        return urlString;
    }



    void handleClient(SocketTcp& serverSocket, SOCKET clientSocket) {
        char recvbuf[512];
        int recvbuflen = sizeof(recvbuf);

        if (clientSocket != INVALID_SOCKET) {
            std::cout << "Client connected." << std::endl;
            bool clientConnected = true;
            while (clientConnected) {

                memset(recvbuf, 0, recvbuflen);

                if (serverSocket.receiveMessage(recvbuf, recvbuflen)) {
                    std::string message(recvbuf);
                    std::cout << "Received: " << message << std::endl;

                    message.erase(std::remove(message.begin(), message.end(), '\n'), message.end());
                    message.erase(std::remove(message.begin(), message.end(), '\r'), message.end());

                    // Controllo messaggi
                    if (message.rfind("login*", 0) == 0) {
                        handleLogin(message.substr(6), serverSocket, clientSocket);
                    }
                    else if (message.rfind("signup*", 0) == 0) {
                        handleSignup(message.substr(7), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getUser*", 0) == 0) {
                        handleGetUser(message.substr(8), serverSocket, clientSocket);
                    }
                    else if (message.rfind("addGame*", 0) == 0) {
                        handleAddGame(message.substr(8), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getGames*", 0) == 0) {
                        handleGetGames(message.substr(9), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getGame*", 0) == 0) {
                        handleGetGame(message.substr(8), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getGameByTitle*", 0) == 0) {
                        handleGetGameByTitle(message.substr(15), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getGameByGenre*", 0) == 0) {
                        handleGetGameByGenre(message.substr(15), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getReview*", 0) == 0) {
                        handleGetReview(message.substr(10), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getReviewByUser*", 0) == 0) {
                        handleGetReviewByUser(message.substr(16), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getReviewByGame*", 0) == 0) {
                        handleGetReviewByGame(message.substr(16), serverSocket, clientSocket);
                    }
                    else if (message.rfind("addReview*", 0) == 0) {
                        handleAddReview(message.substr(10), serverSocket, clientSocket);
                    }
                    else if (message.rfind("addReservation*", 0) == 0) {
                        handleAddReservation(message.substr(15), serverSocket, clientSocket);
                    }
                    else if (message.rfind("addPurchase*", 0) == 0) {
                        handleAddPurchase(message.substr(12), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getReservations*", 0) == 0) {
                        handleGetReservation(message.substr(16), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getAllPurchases*", 0) == 0) {
                        handleGetAllPurchases(message.substr(16), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getPurchases*", 0) == 0) {
                        handleGetPurchases(message.substr(13), serverSocket, clientSocket);
                    }
                    /*
                    else if (message.rfind("getRecommendations*", 0) == 0) {
                        handleGetRecommendations(message.substr(19), serverSocket, clientSocket);
                    }*/
                    else if (message.rfind("updateUser*", 0) == 0) {
                        handleUpdateUser(message.substr(11), serverSocket, clientSocket);
                    }
                    else if (message.rfind("updateGame*", 0) == 0) {
                        handleUpdateGame(message.substr(11), serverSocket, clientSocket);
                    }
                    else if (message.rfind("updateReservation*", 0) == 0) {
                        handleUpdateReservation(message.substr(18), serverSocket, clientSocket);
                    }
                    else if (message.rfind("updatePurchase*", 0) == 0) {
                        handleUpdatePurchase(message.substr(15), serverSocket, clientSocket);
                    }
                    else if (message.rfind("updateReview*", 0) == 0) {
                        handleUpdateReview(message.substr(13), serverSocket, clientSocket);
                    }/*
                    else if (message.rfind("updateRecommendation*", 0) == 0) {
                        handleUpdateRecommendation(message.substr(20), serverSocket, clientSocket);
                    }*/
                    else if (message.rfind("deleteUser*", 0) == 0) {
                        handleDeleteUser(message.substr(11), serverSocket, clientSocket);
                    }
                    else if (message.rfind("deleteGame*", 0) == 0) {
                        handleDeleteGame(message.substr(11), serverSocket, clientSocket);
                    }
                    else if (message.rfind("deleteReservation*", 0) == 0) {
                        handleDeleteReservation(message.substr(18), serverSocket, clientSocket);
                    }
                    else if (message.rfind("deletePurchase*", 0) == 0) {
                        handleDeletePurchase(message.substr(15), serverSocket, clientSocket);
                    }
                    else if (message.rfind("deleteReview*", 0) == 0) {
                        handleDeleteReview(message.substr(13), serverSocket, clientSocket);
                    }/*
                    else if (message.rfind("deleteRecommendation*", 0) == 0) {
                        handleDeleteRecommendation(message.substr(20), serverSocket, clientSocket);
                    }*/
                    else if (message == "exit") {
                        const char* response = "Goodbye!";
                        if (!serverSocket.sendMessage(response, clientSocket)) {
                            throw HandlerException("Failed to send response.");
                        }
                        clientConnected = false;
                    }
                    else {
                        std::string error = generateJson("", "Unknown command", true);
                        serverSocket.sendMessage(error.c_str(), clientSocket);
                    }
                    memset(recvbuf, 0, recvbuflen);
                }
                else {
                    // Gestione la disconnessione del client
                    std::string error = generateJson("", "Client disconnected.", true);
                    serverSocket.sendMessage(error.c_str(), clientSocket);
                    clientConnected = false;
                }
            }
            // Chiusura della connessione e pulizia delle risorse
            closesocket(clientSocket);
        }
        else {
            throw HandlerException("Failed to accept client connection.");
        }
    }

}
