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
#include "../exceptions/GetRecommendationsException.h"
#include "../exceptions/UserNotFoundException.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>


class MongoDB {
private:
    mongocxx::instance* instance;
    mongocxx::database database;
    mongocxx::collection userCollection;
    mongocxx::collection gameCollection;
    mongocxx::collection reservationCollection;
    mongocxx::collection recommendationCollection;

    static MongoDB* INSTANCE;

    MongoDB();

public:
    static MongoDB* getInstance();
    void connectDB(const std::string& hostname, const std::string& port, const std::string& databaseName, const std::string& username, const std::string& password);
    void signup(const std::string& username, const std::string& pwd, const std::string& imageUrl) noexcept(false);
    void login(const std::string& username, const std::string& pwd) noexcept(false);
    void addGame(const std::string& title, const std::string& genre, const std::string& release_date, const std::string& developer, double price, int stock, const std::string& description, const std::string& imageUrl);
    std::vector<bsoncxx::document::value> getGames();
    bsoncxx::document::value getGame(const std::string& game_id);
    bsoncxx::document::value getReview(const std::string& review_id);
    void addReview(const std::string& username, const std::string& game_id, const std::string& review_text, int rating);
    void addReservation(const std::string& username, const std::string& game_id);
    bsoncxx::document::value getReservation(const std::string& reservation_id);
    std::vector<bsoncxx::document::value> getRecommendations(const std::string& username);
    std::chrono::system_clock::time_point convertToDate(const std::string& date);
   //TODO: Implementare funzioni update
    //void updateUser(const std::string& username, const std::string& new_password, const std::string& new_imageUrl);
    //void updateGame(const std::string& game_id, const std::string& new_title, const std::string& new_genre, const std::string& new_release_date, const std::string& new_developer, double new_price, int new_stock, const std::string& new_description, const std::string& new_imageUrl);
    //void updateRecommendation(const std::string& username, const std::vector<std::string>& new_recommendations);
    //void updateReview(const std::string& review_id, const std::string& new_text, int new_rating);
    //void updateReservation(const std::string& reservation_id, const std::string& new_game_id);
};


#endif //VIDEOGAME_STORE_MONGODB_H