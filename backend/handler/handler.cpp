#include "Handler.h"
#include <iostream>
#include <sstream>

//TODO: Implementare la logica del jwt Token
// TODO: Implemendare sistema di raccomandazioni

namespace handler {

    void handleLogin(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, password;
        if (std::getline(iss, username, '/') && std::getline(iss, password, '/')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->login(username, password);
                const char* response = "Login successful";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const LoginException& e) {
                std::string error = "Login failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Login failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid login format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleSignup(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, password, imageUrl;
        if (std::getline(iss, username, '/') && std::getline(iss, password, '/') && std::getline(iss, imageUrl, '/')) {
            try {
                if (imageUrl.empty()) {
                    imageUrl = "https://example.com/default_image.png"; //TODO: Cambiare immagine facoltativa
                }
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->signup(username, password,imageUrl);
                const char* response = "Signup successful";
                //TODO: Ritornare jwt
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const SignupException& e) {
                std::string error = "Signup failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Signup failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid signup format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleAddGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string title, genre, release_date, developer, price_str, stock_str, description, imageUrl;
        if (std::getline(iss, title, '/') && std::getline(iss, genre, '/') && std::getline(iss, release_date, '/') &&
            std::getline(iss, developer, '/') && std::getline(iss, price_str, '/') && std::getline(iss, stock_str, '/') &&
            std::getline(iss, description, '/') && std::getline(iss, imageUrl, '/')) {

            try {
                double price = std::stod(price_str);
                int stock = std::stoi(stock_str);
                if (imageUrl.empty()) {
                    imageUrl = "https://example.com/default_image.png"; //TODO: Cambiare immagine facoltativa
                }
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->addGame(title, genre, release_date, developer, price, stock, description, imageUrl);
                const char* response = "Game added successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const CreateGameException& e) {
                std::string error = "Add game failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Add game failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid add game format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleGetGames(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        try {
            MongoDB* mongoDb = MongoDB::getInstance();
            nlohmann::json games = mongoDb->getGames();
            std::string response = games.dump(4); //Converte il JSON in una stringa formattata con indentazione
            serverSocket.sendMessage(response.c_str(), clientSocket);
        }
        catch (const GetGameException& e) {
            std::string error = "Get games failed: " + std::string(e.what());
            serverSocket.sendMessage(error.c_str(), clientSocket);
            return;
        }
        catch (const std::exception& e) {
            std::string error = "Get games failed: " + std::string(e.what());
            serverSocket.sendMessage(error.c_str(), clientSocket);
            return;
        }
    }

    void handleGetGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string game_id;
        if (std::getline(iss, game_id, '/')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                nlohmann::json games = mongoDb->getGame(game_id);
                std::string response = games.dump(4); //Converte il JSON in una stringa formattata con indentazione
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const GetGameException& e) {
                std::string error = "Get game failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid get game format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleGetGameByTitle(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string title;
        if (std::getline(iss, title, '/')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                nlohmann::json games = mongoDb->getGameByTitle(title);
                std::string response = games.dump(4); //Converte il JSON in una stringa formattata con indentazione
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const GetGameException& e) {
                std::string error = "Get game failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid get game format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleGetReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string review_id;
        if (std::getline(iss, review_id, '/')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                nlohmann::json review = mongoDb->getReview(review_id);
                std::string response = review.dump(4); //Converte il JSON in una stringa formattata con indentazione
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const ReviewException& e) {
                std::string error = "Get review failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Get review failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid get review format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleGetReviewByUser(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string user;
        if (std::getline(iss, user, '/')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                nlohmann::json review = mongoDb->getReviewByUser(user);
                std::string response = review.dump(4); //Converte il JSON in una stringa formattata con indentazione
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const ReviewException& e) {
                std::string error = "Get review failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const UserNotFoundException& e) {
                std::string error = "Get review failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid get review format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleGetReviewByGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string game_title;
        if (std::getline(iss, game_title, '/')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                nlohmann::json review = mongoDb->getReviewByGame(game_title);
                std::string response = review.dump(4); //Converte il JSON in una stringa formattata con indentazione
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const ReviewException& e) {
                std::string error = "Get review failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const GetGameException& e) {
                std::string error = "Get review failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid get review format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleAddReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, review_text, rating_str;
        if (std::getline(iss, username, '/') && std::getline(iss, game_title, '/') &&
            std::getline(iss, review_text, '/') && std::getline(iss, rating_str, '/')) {

            try {
                int rating = std::stoi(rating_str);
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->addReview(username, game_title, review_text, rating);
                const char* response = "Review added successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const UserNotFoundException& e) {
                std::string error = "Add review failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const GetGameException& e) {
                std::string error = "Add review failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const CreateReviewException& e) {
                std::string error = "Add review failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid add review format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleAddReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, num_copies_str;
        if (std::getline(iss, username, '/') && std::getline(iss, game_title, '/') && std::getline(iss, num_copies_str, '/')) {
            try {
                int num_copies = std::stoi(num_copies_str);
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->addReservation(username, game_title, num_copies);
                const char* response = "Reservation added successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const UserNotFoundException& e) {
                std::string error = "Add reservation failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const GetGameException& e) {
                std::string error = "Add reservation failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const CreateReservationException e) {
                std::string error = "Add reservation failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid add reservation format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleAddPurchase(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, num_copies_str;
        if (std::getline(iss, username, '/') && std::getline(iss, game_title, '/') && std::getline(iss, num_copies_str, '/')) {
            try {
                int num_copies = std::stoi(num_copies_str);
                MongoDB* mongoDb = MongoDB::getInstance();
                // Verifica se il gioco è disponibile per l'acquisto
                if (!isGameAvailableForPurchase(game_title, num_copies)) {
                    throw PurchaseException("Not enough copies available for purchase");
                }
                // Effettua l'acquisto
                mongoDb->addPurchase(username, game_title, num_copies);
                // Rimuove le prenotazioni se l'utente ha già prenotato questo gioco
                removeReservationsForGame(username, game_title, num_copies);
                const char* response = "Purchase added successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const UserNotFoundException& e) {
                std::string error = "Add purchase failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const GetGameException& e) {
                std::string error = "Add purchase failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const PurchaseException& e) {
                std::string error = "Add purchase failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid add purchase format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleGetReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username;
        if (std::getline(iss, username, '/')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                nlohmann::json review = mongoDb->getReservations(username);
                std::string response = review.dump(4); //Converte il JSON in una stringa formattata con indentazione
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const UserNotFoundException& e) {
                std::string error = "Get reservation failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const ReservationException& e) {
                std::string error = "Get reservation failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid get reservation format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleGetPurchases(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username;
        if (std::getline(iss, username, '/')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                nlohmann::json purchases = mongoDb->getPurchases(username);
                std::string response = purchases.dump(4);
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const UserNotFoundException& e) {
                std::string error = "Get purchases failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const PurchaseException& e) {
                std::string error = "Get purchases failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid get purchases format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    //TODO: Implementare handleGetRecommendations
    /*
    void handleGetRecommendations(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username;
        if (std::getline(iss, username, '/')) {
            try {
                //std::vector<bsoncxx::document::value> recommendations = getRecommendations(username);
                std::string response = "Recommendations:";  // Convert recommendations to string
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const std::exception& e) {
                std::string error = "Get recommendations failed: " + std::string(e.what());
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
        std::string username, password, imageUrl;
        if (std::getline(iss, username, '/') && std::getline(iss, password, '/') && std::getline(iss, imageUrl, '/')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->updateUser(username, password, imageUrl);
                const char* response = "User modified successful";
                //TODO: Ritornare jwt
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const UserNotFoundException& e) {
                std::string error = "Modify user failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const HandlerException& e) {
                std::string error = "Modify user failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Modify user failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid Modify user format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleUpdateGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string title, genre, release_date, developer, price_str, stock_str, description, imageUrl;
        if (std::getline(iss, title, '/') && std::getline(iss, genre, '/') && std::getline(iss, release_date, '/') &&
            std::getline(iss, developer, '/') && std::getline(iss, price_str, '/') && std::getline(iss, stock_str, '/') &&
            std::getline(iss, description, '/') && std::getline(iss, imageUrl, '/')) {

            try {
                double price = std::stod(price_str);
                int stock = std::stoi(stock_str);
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->updateGame(title, genre, release_date, developer, price, stock, description, imageUrl);
                const char* response = "Game updated successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const GetGameException& e) {
                std::string error = "Update game failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const HandlerException& e) {
                std::string error = "Update game failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Update game failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid update game format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleUpdateReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, newNumCopies_str;
        if (std::getline(iss, username, '/') && std::getline(iss, game_title, '/') && std::getline(iss, newNumCopies_str, '/')) {
            try {
                int newNumCopies = std::stoi(newNumCopies_str);
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->updateReservation(username, game_title, newNumCopies);
                const char* response = "Reservation updated successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const ReservationException& e) {
                std::string error = "Update reservation failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const mongocxx::exception& e) {
                std::string error = "Update reservation failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Update reservation failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid update reservation format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleUpdatePurchase(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, newNumCopies_str, purchase_id;
        if (std::getline(iss, username, '/') && std::getline(iss, game_title, '/') && std::getline(iss, newNumCopies_str, '/') && std::getline(iss, purchase_id, '/')) {
            try {
                int newNumCopies = std::stoi(newNumCopies_str);
                MongoDB* mongoDb = MongoDB::getInstance();
                // Verifica se il gioco è disponibile per l'acquisto
                // Altrimenti, solleva un'eccezione PurchaseException
                if (!isGameAvailableForPurchase(game_title, newNumCopies)) {
                    throw PurchaseException("Not enough copies available for purchase");
                }
                // Aggiorna l'acquisto
                mongoDb->updatePurchase(username, game_title, newNumCopies, purchase_id);
                const char* response = "Purchase updated successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const ReservationException& e) {
                std::string error = "Update purchase failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const mongocxx::exception& e) {
                std::string error = "Update purchase failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Update purchase failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid update purchase format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }


    void handleUpdateReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, newReviewText, newRating_str;
        if (std::getline(iss, username, '/') && std::getline(iss, game_title, '/') &&
            std::getline(iss, newReviewText, '/') && std::getline(iss, newRating_str, '/')) {

            try {
                int newRating = std::stoi(newRating_str);
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->updateReview(username, game_title, newReviewText, newRating);
                const char* response = "Review updated successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const ReviewException& e) {
                std::string error = "Update review failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const mongocxx::exception& e) {
                std::string error = "Update review failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Update review failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid update review format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleDeleteUser(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username;
        if (std::getline(iss, username, '/')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->deleteUser(username);
                const char* response = "User deleted successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const UserNotFoundException& e) {
                std::string error = "Delete User failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Delete User failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid delete user format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleDeleteGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string game_title;
        if (std::getline(iss, game_title, '/')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->deleteGame(game_title);
                const char* response = "game deleted successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (GetGameException& e) {
                std::string error = "Delete Game failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Delete game failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid delete game format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleDeleteReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title;
        if (std::getline(iss, username, '/') && std::getline(iss, game_title, '/')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                mongoDb->deleteReservation(username, game_title);
                const char* response = "reservation deleted successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (GetGameException& e) {
                std::string error = "Delete reservation failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Delete reservation failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid delete reservation format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleDeletePurchase(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title, purchase_id;
        if (std::getline(iss, username, '/') && std::getline(iss, game_title, '/') && std::getline(iss, purchase_id, '/')) {
            try {
                MongoDB* mongoDb = MongoDB::getInstance();
                // Elimina l'acquisto
                mongoDb->deletePurchase(username, game_title, purchase_id);
                const char* response = "Purchase deleted successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (GetGameException& e) {
                std::string error = "Delete purchase failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
            catch (const std::exception& e) {
                std::string error = "Delete purchase failed: " + std::string(e.what());
                serverSocket.sendMessage(error.c_str(), clientSocket);
                return;
            }
        }
        else {
            const char* response = "Invalid delete purchase format";
            serverSocket.sendMessage(response, clientSocket);
            return;
        }
    }

    void handleDeleteReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title;
        if (std::getline(iss, username, '/') && std::getline(iss, game_title, '/')) {
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
    }

    //TODO: Da implementare handleDeleteRecommendation
    /*
    void handleDeleteRecommendation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_title;
        if (std::getline(iss, username, '/') && std::getline(iss, game_title, '/')) {
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
        if (reserved < num_copies_purchased) {
            mongoDb->deleteReservation(username, game_title);
        }
        else {
            int reserved_new = reserved - num_copies_purchased;
            mongoDb->updateReservation(username, game_title, reserved_new);
        }
        return true;
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
                    if (message.rfind("login/", 0) == 0) {
                        handleLogin(message.substr(6), serverSocket, clientSocket);
                    }
                    else if (message.rfind("signup/", 0) == 0) {
                        handleSignup(message.substr(7), serverSocket, clientSocket);
                    }
                    else if (message.rfind("addGame/", 0) == 0) {
                        handleAddGame(message.substr(8), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getGames/", 0) == 0) {
                        handleGetGames(message.substr(9), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getGame/", 0) == 0) {
                        handleGetGame(message.substr(8), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getGameByTitle/", 0) == 0) {
                        handleGetGameByTitle(message.substr(15), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getReview/", 0) == 0) {
                        handleGetReview(message.substr(10), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getReviewByUser/", 0) == 0) {
                        handleGetReviewByUser(message.substr(16), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getReviewByGame/", 0) == 0) {
                        handleGetReviewByGame(message.substr(16), serverSocket, clientSocket);
                    }
                    else if (message.rfind("addReview/", 0) == 0) {
                        handleAddReview(message.substr(10), serverSocket, clientSocket);
                    }
                    else if (message.rfind("addReservation/", 0) == 0) {
                        handleAddReservation(message.substr(15), serverSocket, clientSocket);
                    }
                    else if (message.rfind("addPurchase/", 0) == 0) {
                        handleAddPurchase(message.substr(12), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getReservations/", 0) == 0) {
                        handleGetReservation(message.substr(16), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getPurchases/", 0) == 0) {
                        handleGetPurchases(message.substr(13), serverSocket, clientSocket);
                    }
                    /*
                    else if (message.rfind("getRecommendations/", 0) == 0) {
                        handleGetRecommendations(message.substr(19), serverSocket, clientSocket);
                    }*/
                    else if (message.rfind("updateUser/", 0) == 0) {
                        handleUpdateUser(message.substr(11), serverSocket, clientSocket);
                    }
                    else if (message.rfind("updateGame/", 0) == 0) {
                        handleUpdateGame(message.substr(11), serverSocket, clientSocket);
                    }
                    else if (message.rfind("updateReservation/", 0) == 0) {
                        handleUpdateReservation(message.substr(18), serverSocket, clientSocket);
                    }
                    else if (message.rfind("updatePurchase/", 0) == 0) {
                        handleUpdatePurchase(message.substr(15), serverSocket, clientSocket);
                    }
                    else if (message.rfind("updateReview/", 0) == 0) {
                        handleUpdateReview(message.substr(13), serverSocket, clientSocket);
                    }/*
                    else if (message.rfind("updateRecommendation/", 0) == 0) {
                        handleUpdateRecommendation(message.substr(20), serverSocket, clientSocket);
                    }*/
                    else if (message.rfind("deleteUser/", 0) == 0) {
                        handleDeleteUser(message.substr(11), serverSocket, clientSocket);
                    }
                    else if (message.rfind("deleteGame/", 0) == 0) {
                        handleDeleteGame(message.substr(11), serverSocket, clientSocket);
                    }
                    else if (message.rfind("deleteReservation/", 0) == 0) {
                        handleDeleteReservation(message.substr(18), serverSocket, clientSocket);
                    }
                    else if (message.rfind("deletePurchase/", 0) == 0) {
                        handleDeletePurchase(message.substr(15), serverSocket, clientSocket);
                    }
                    else if (message.rfind("deleteReview/", 0) == 0) {
                        handleDeleteReview(message.substr(13), serverSocket, clientSocket);
                    }/*
                    else if (message.rfind("deleteRecommendation/", 0) == 0) {
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
                        const char* response = "Unknown command";
                        if (!serverSocket.sendMessage(response, clientSocket)) {
                            throw HandlerException("Failed to send response.");
                        }
                    }
                    memset(recvbuf, 0, recvbuflen);
                }
                else {
                    // Gestione la disconnessione del client
                    std::cerr << "Client disconnected." << std::endl;
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
