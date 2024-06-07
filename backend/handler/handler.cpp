#include "Handler.h"
#include <iostream>
#include <sstream>

//TODO: Testare connessioni con MongoDB.cpp
//TODO: Implementare la logica del jwt Token

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
        std::string username, game_id;
        if (std::getline(iss, username, '/') && std::getline(iss, game_id, '/')) {
            try {
                //addReservation(username, game_id);
                const char* response = "Reservation added successfully";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const std::exception& e) {
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

    void handleGetReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string reservation_id;
        if (std::getline(iss, reservation_id, '/')) {
            try {
                //bsoncxx::document::value reservation = getReservation(reservation_id);
                std::string response = "Reservation details:";  // Convert reservation to string
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const std::exception& e) {
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
                        handleAddReservation(message.substr(14), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getReservation/", 0) == 0) {
                        handleGetReservation(message.substr(14), serverSocket, clientSocket);
                    }
                    else if (message.rfind("getRecommendations/", 0) == 0) {
                        handleGetRecommendations(message.substr(19), serverSocket, clientSocket);
                    }
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
                    // Gestisco la disconnessione del client
                    std::cerr << "Client disconnected." << std::endl;
                    clientConnected = false;
                }
            }
            // Chiudo la connessione e pulisci le risorse
            closesocket(clientSocket);
        }
        else {
            throw HandlerException("Failed to accept client connection.");
        }
    }

}
