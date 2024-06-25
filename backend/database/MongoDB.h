#ifndef VIDEOGAME_STORE_MONGODB_H
#define VIDEOGAME_STORE_MONGODB_H

#include "list"
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include "../exceptions/LoginException.h"
#include "../exceptions/SignupException.h"
#include "../exceptions/UploadException.h"
#include "../exceptions/CreateGameException.h"
#include "../exceptions/CreateReservationException.h"
#include "../exceptions/CreateReviewException.h"
#include "../exceptions/GetGameException.h"
#include "../exceptions/UserNotFoundException.h"
#include "../exceptions/ReviewException.h"
#include "../exceptions/ReservationException.h"
#include "../exceptions/HandlerException.h"
#include "../exceptions/PurchaseException.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <set>


class MongoDB {
private:
    mongocxx::instance* instance;
    mongocxx::database database;
    mongocxx::collection userCollection;
    mongocxx::collection gameCollection;
    mongocxx::collection reservationCollection;
    mongocxx::collection purchaseCollection;

    static MongoDB* INSTANCE;

    MongoDB();

public:
    static MongoDB* getInstance();
    void connectDB(const std::string& hostname, const std::string& port, const std::string& databaseName, const std::string& username, const std::string& password);
    void signup(const std::string& username, const std::string& pwd, const std::string& imageUrl) noexcept(false);
    void login(const std::string& username, const std::string& pwd) noexcept(false);
    void addGame(const std::string& title, const std::string& genre, const std::string& release_date, const std::string& developer, double price, int stock, const std::string& description, const std::string& imageUrl);
    nlohmann::json MongoDB::getUser(const std::string& username);
    nlohmann::json MongoDB::getAllUsers();
    nlohmann::json getGames();
    nlohmann::json getGame(const std::string& game_id);
    nlohmann::json MongoDB::getGameByTitle(const std::string& game_id);
    nlohmann::json MongoDB::getGameByGenre(const std::string& genre);
    nlohmann::json MongoDB::getReview(const std::string& review_id);
    nlohmann::json MongoDB::getReviewByUser(const std::string& username);
    nlohmann::json MongoDB::getReviewByGame(const std::string& game_title);
    nlohmann::json MongoDB::getUserPreferredGames(const std::string& username);
    void addReview(const std::string& username, const std::string& game_title, const std::string& review_text, int rating);
    void addReservation(const std::string& username, const std::string& game_id, int num_copies);
    void addPurchase(const std::string& username, const std::string& game_title, int num_copies);
    nlohmann::json getReservations(const std::string& username);
    nlohmann::json MongoDB::getAllPurchases();
    nlohmann::json getPurchases(const std::string& username);
    std::chrono::system_clock::time_point convertToDate(const std::string& date);
    void updateUser(const std::string& username, const std::string& new_password, const std::string& new_imageUrl);
    void updateGame(const std::string& title, const std::string& newGenre, const std::string& newReleaseDate, const std::string& newDeveloper, double newPrice, int newStock, const std::string& newDescription, const std::string& newImageUrl);
    void updateReview(const std::string& username, const std::string& game_title, const std::string& newReviewText, int newRating);
    void updateReservation(const std::string& username, const std::string& game_title, int newNumCopies);
    void updatePurchase(int newNumCopies, const std::string& purchase_id);
    void deleteUser(const std::string& username);
    void deleteGame(const std::string& game_title);
    void deleteReservation(const std::string& reservationId);
    void deletePurchase(const std::string& purchase_id);
    void deleteReview(const std::string& username, const std::string& game_title);
};


#endif //VIDEOGAME_STORE_MONGODB_H