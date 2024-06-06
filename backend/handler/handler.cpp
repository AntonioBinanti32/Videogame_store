#include "Handler.h"
#include <iostream>
#include <sstream>

namespace handler {
    /* HANDLER DI TEST
    void handleLogin(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        // Parse del messaggio di login per ottenere username e password
        std::istringstream iss(message);
        std::string username, password;
        if (std::getline(iss, username, '/') && std::getline(iss, password, '/')) {
            std::cout << "Handling login for username: " << username << ", password: " << password << std::endl;
            const char* response = "Login successful \n";
            serverSocket.sendMessage(response, clientSocket);
        }
        else {
            throw new HandlerException("Invalid login format");
            const char* response = "Invalid login format";
            serverSocket.sendMessage(response, clientSocket);
        }
    }

    void handleSignup(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        // Parse del messaggio di registrazione per ottenere username e password
        std::istringstream iss(message);
        std::string username, password;
        if (std::getline(iss, username, '/') && std::getline(iss, password, '/')) {
            std::cout << "Handling signup for username: " << username << ", password: " << password << std::endl;
            const char* response = "Signup successful \n";
            serverSocket.sendMessage(response, clientSocket);
        }
        else {
            throw new HandlerException("Invalid signup format");
            const char* response = "Invalid signup format";
            serverSocket.sendMessage(response, clientSocket);
        }
    }

    void handleAddGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        // Parse del messaggio di addGame per ottenere i dati
        std::istringstream iss(message);
        std::string data1, data2;
        if (std::getline(iss, data1, '/') && std::getline(iss, data2, '/')) {
            std::cout << "Handling addGame for data1: " << data1 << ", data2: " << data2 << std::endl;
            const char* response = "Game added successfully \n";
            serverSocket.sendMessage(response, clientSocket);
        }
        else {
            throw new HandlerException("Invalid addGame format");
            const char* response = "Invalid addGame format";
            serverSocket.sendMessage(response, clientSocket);
        }
    }*/

    void handleLogin(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, password;
        if (std::getline(iss, username, '/') && std::getline(iss, password, '/')) {
            try {
                //login(username, password);
                const char* response = "Login successful \n";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const std::exception& e) {
                std::string error = "Login failed: " + std::string(e.what()) + "\n";
                serverSocket.sendMessage(error.c_str(), clientSocket);
            }
        }
        else {
            const char* response = "Invalid login format";
            serverSocket.sendMessage(response, clientSocket);
        }
    }

    void handleSignup(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, password, imageUrl;
        if (std::getline(iss, username, '/') && std::getline(iss, password, '/') && std::getline(iss, imageUrl, '/')) {
            try {
                //signup(username, password, imageUrl);
                const char* response = "Signup successful \n";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const std::exception& e) {
                std::string error = "Signup failed: " + std::string(e.what()) + "\n";
                serverSocket.sendMessage(error.c_str(), clientSocket);
            }
        }
        else {
            const char* response = "Invalid signup format";
            serverSocket.sendMessage(response, clientSocket);
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
                //addGame(title, genre, release_date, developer, price, stock, description, imageUrl);
                const char* response = "Game added successfully \n";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const std::exception& e) {
                std::string error = "Add game failed: " + std::string(e.what()) + "\n";
                serverSocket.sendMessage(error.c_str(), clientSocket);
            }
        }
        else {
            const char* response = "Invalid add game format";
            serverSocket.sendMessage(response, clientSocket);
        }
    }

    void handleGetGames(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        try {
            //std::vector<bsoncxx::document::value> games = getGames();
            std::string response = "Games list: \n";  // Convert games to string
            serverSocket.sendMessage(response.c_str(), clientSocket);
        }
        catch (const std::exception& e) {
            std::string error = "Get games failed: " + std::string(e.what()) + "\n";
            serverSocket.sendMessage(error.c_str(), clientSocket);
        }
    }

    void handleGetGame(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string game_id;
        if (std::getline(iss, game_id, '/')) {
            try {
                //bsoncxx::document::value game = getGame(game_id);
                std::string response = "Game details: \n";  // Convert game to string
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const std::exception& e) {
                std::string error = "Get game failed: " + std::string(e.what()) + "\n";
                serverSocket.sendMessage(error.c_str(), clientSocket);
            }
        }
        else {
            const char* response = "Invalid get game format";
            serverSocket.sendMessage(response, clientSocket);
        }
    }

    void handleGetReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string review_id;
        if (std::getline(iss, review_id, '/')) {
            try {
                //bsoncxx::document::value review = getReview(review_id);
                std::string response = "Review details: \n";  // Convert review to string
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const std::exception& e) {
                std::string error = "Get review failed: " + std::string(e.what()) + "\n";
                serverSocket.sendMessage(error.c_str(), clientSocket);
            }
        }
        else {
            const char* response = "Invalid get review format";
            serverSocket.sendMessage(response, clientSocket);
        }
    }

    void handleAddReview(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_id, review_text, rating_str;
        if (std::getline(iss, username, '/') && std::getline(iss, game_id, '/') &&
            std::getline(iss, review_text, '/') && std::getline(iss, rating_str, '/')) {

            try {
                int rating = std::stoi(rating_str);
                //addReview(username, game_id, review_text, rating);
                const char* response = "Review added successfully \n";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const std::exception& e) {
                std::string error = "Add review failed: " + std::string(e.what()) + "\n";
                serverSocket.sendMessage(error.c_str(), clientSocket);
            }
        }
        else {
            const char* response = "Invalid add review format";
            serverSocket.sendMessage(response, clientSocket);
        }
    }

    void handleAddReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username, game_id;
        if (std::getline(iss, username, '/') && std::getline(iss, game_id, '/')) {
            try {
                //addReservation(username, game_id);
                const char* response = "Reservation added successfully \n";
                serverSocket.sendMessage(response, clientSocket);
            }
            catch (const std::exception& e) {
                std::string error = "Add reservation failed: " + std::string(e.what()) + "\n";
                serverSocket.sendMessage(error.c_str(), clientSocket);
            }
        }
        else {
            const char* response = "Invalid add reservation format";
            serverSocket.sendMessage(response, clientSocket);
        }
    }

    void handleGetReservation(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string reservation_id;
        if (std::getline(iss, reservation_id, '/')) {
            try {
                //bsoncxx::document::value reservation = getReservation(reservation_id);
                std::string response = "Reservation details: \n";  // Convert reservation to string
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const std::exception& e) {
                std::string error = "Get reservation failed: " + std::string(e.what()) + "\n";
                serverSocket.sendMessage(error.c_str(), clientSocket);
            }
        }
        else {
            const char* response = "Invalid get reservation format";
            serverSocket.sendMessage(response, clientSocket);
        }
    }

    void handleGetRecommendations(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket) {
        std::istringstream iss(message);
        std::string username;
        if (std::getline(iss, username, '/')) {
            try {
                //std::vector<bsoncxx::document::value> recommendations = getRecommendations(username);
                std::string response = "Recommendations: \n";  // Convert recommendations to string
                serverSocket.sendMessage(response.c_str(), clientSocket);
            }
            catch (const std::exception& e) {
                std::string error = "Get recommendations failed: " + std::string(e.what()) + "\n";
                serverSocket.sendMessage(error.c_str(), clientSocket);
            }
        }
        else {
            const char* response = "Invalid get recommendations format";
            serverSocket.sendMessage(response, clientSocket);
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
                    else if (message.rfind("getReview/", 0) == 0) {
                        handleGetReview(message.substr(10), serverSocket, clientSocket);
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
                        const char* response = "Goodbye! \n";
                        if (!serverSocket.sendMessage(response, clientSocket)) {
                            throw new HandlerException("Failed to send response.");
                        }
                        clientConnected = false;
                    }
                    else {
                        const char* response = "Unknown command \n";
                        if (!serverSocket.sendMessage(response, clientSocket)) {
                            throw new HandlerException("Failed to send response.");
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
            throw new HandlerException("Failed to accept client connection.");
        }
    }

}
