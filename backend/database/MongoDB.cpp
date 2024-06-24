#include "MongoDB.h"


MongoDB* MongoDB::INSTANCE;

// Ottengo un'istanza di mongocxx da usare per comunicare col database
MongoDB::MongoDB() {
    instance = new mongocxx::instance();
}

// Pattern Singleton affiché venga creata una sola istanza della classe MongoDB
MongoDB* MongoDB::getInstance() {
    if (INSTANCE == nullptr) {
        INSTANCE = new MongoDB();
    }
    return INSTANCE;
}

// Connessione al database, specificando dove si trova il server, le credenziali di accesso e quale database usare
void MongoDB::connectDB(const std::string& hostname, const std::string& port, const std::string& databaseName, const std::string& username, const std::string& password) {
    auto* uri = new mongocxx::uri("mongodb://" + username + ":" + password + "@" + hostname + ":" + port);
    auto* client = new mongocxx::client(*uri);
    database = (*client)[databaseName];

    // Riferimento alle collezioni
    try {
        userCollection = database["users"];
        gameCollection = database["games"];
        reservationCollection = database["reservations"];
        purchaseCollection = database["purchases"];
        recommendationCollection = database["recommendations"];
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

// Registrazione dell'utente (viene inserito un nuovo documento nellla collezione user).
// La funzione può lanciare un'eccezione di tipo diverso da std::bad_alloc (indicato dal noexcept(false)).
// Se è già presente un utente con quell'username, viene lanciata un eccezione
void MongoDB::signup(const std::string& username, const std::string& pwd, const std::string& imageUrl) noexcept(false) {
    // Creazione nuovo documento di BSON
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("username", username),
        bsoncxx::builder::basic::kvp("password", pwd),
        bsoncxx::builder::basic::kvp("image_url", imageUrl),
        bsoncxx::builder::basic::kvp("created_at", bsoncxx::types::b_date(std::chrono::system_clock::now()))
    );
    try {
        // Inserimento documento creato precedentemente nella collezione userCollection
        userCollection.insert_one(doc_value);
    }
    catch (const mongocxx::bulk_write_exception& e) {        
        throw SignupException("Utente gia' registrato");
    }
    catch (const mongocxx::exception& e) {
        std::cerr << "MongoDB exception: " << e.what() << std::endl;
        throw;
    }
    catch (const std::exception& e) {
        std::cerr << "General exception: " << e.what() << std::endl;
        throw;
    }
}

// Login dell'utente. Se non è presente nel database lancio un eccezione di Login Exception
void MongoDB::login(const std::string& username, const std::string& pwd) noexcept(false) {
    // Creazione documento BSON
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("username", username),
        bsoncxx::builder::basic::kvp("password", pwd)
    );
    // Ricerca documento nella collezione
    auto find_one_result = userCollection.find_one(doc_value);
    if (!find_one_result) {
        throw LoginException("User non presente o password non corretta");
    }
}


// Aggiunta di un nuovo gioco
void MongoDB::addGame(const std::string& title, const std::string& genre, const std::string& release_date, const std::string& developer, double price, int stock, const std::string& description, const std::string& imageUrl) { 
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("title", title),
        bsoncxx::builder::basic::kvp("genre", genre),
        bsoncxx::builder::basic::kvp("release_date", bsoncxx::types::b_date(convertToDate(release_date))),
        bsoncxx::builder::basic::kvp("developer", developer),
        bsoncxx::builder::basic::kvp("price", price),
        bsoncxx::builder::basic::kvp("stock", stock),
        bsoncxx::builder::basic::kvp("description", description),
        bsoncxx::builder::basic::kvp("image_url", imageUrl)
    );
    try {
        gameCollection.insert_one(doc_value);
    }
    catch (mongocxx::bulk_write_exception& e) {
        throw CreateGameException("Errore durante l'aggiunta del gioco");
    }
}

nlohmann::json MongoDB::getUser(const std::string& username) {
    try {
        auto result = userCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
        if (!result) {
            throw UserNotFoundException("Errore durante il recupero del gioco: Gioco non trovato");
        }
        return nlohmann::json::parse(bsoncxx::to_json(*result));
    }
    catch (std::exception& e) {
        throw UserNotFoundException("Errore durante il recupero del gioco");
    }
}

nlohmann::json MongoDB::getAllUsers() {
    nlohmann::json users;
    try {
        auto cursor = userCollection.find({});
        for (auto&& doc : cursor) {
            users.push_back(nlohmann::json::parse(bsoncxx::to_json(doc)));
        }
    }
    catch (std::exception& e) {
        throw  UserNotFoundException("Errore durante il recupero degli utenti");
    }
    return users;
}

// Ottenimento della lista dei giochi
nlohmann::json MongoDB::getGames() {
    nlohmann::json games;
    try {
        auto cursor = gameCollection.find({});
        for (auto&& doc : cursor) {
            games.push_back(nlohmann::json ::parse(bsoncxx::to_json(doc)));
        }
    }
    catch (std::exception& e) {
        throw  GetGameException("Errore durante il recupero dei giochi");
    }
    return games;
}

// Ottenimento di un gioco specifico
nlohmann::json MongoDB::getGame(const std::string& game_id) {
    try {
        auto result = gameCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", bsoncxx::oid(game_id))));
        if (!result) {
            throw GetGameException("Errore durante il recupero del gioco: Gioco non trovato");
        }
        return nlohmann::json::parse(bsoncxx::to_json(*result));
    }
    catch (std::exception& e) {
        throw GetGameException("Errore durante il recupero del gioco");
    }
}

nlohmann::json MongoDB::getGameByTitle(const std::string& title) {
    try {
        auto result = gameCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("title", title)));
        if (!result) {
            throw GetGameException("Errore durante il recupero del gioco: Gioco non trovato");
        }
        return nlohmann::json::parse(bsoncxx::to_json(*result));
    }
    catch (std::exception& e) {
        throw GetGameException("Errore durante il recupero del gioco");
    }
}

nlohmann::json MongoDB::getGameByGenre(const std::string& genre) { //TODO: Da sistemare
    try{
        auto filter = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("genre", genre));

        auto cursor = gameCollection.find(filter.view());

        nlohmann::json gamesArray = nlohmann::json::array();

        for (auto&& doc : cursor) {
            gamesArray.push_back(nlohmann::json::parse(bsoncxx::to_json(doc)));
        }

        return gamesArray;
    }
    catch (std::exception& e) {
        throw GetGameException("Errore durante il recupero dei giochi");
    }
}

void MongoDB::addReview(const std::string& username, const std::string& game_title, const std::string& review_text, int rating) {
    try {
        auto game_doc = gameCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("title", game_title)));
        if (!game_doc) {
            throw GetGameException("Gioco non trovato");
        }
        auto game_id = (*game_doc)["_id"].get_oid().value;

        auto user_doc = userCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
        if (!user_doc) {
            throw UserNotFoundException("Utente non trovato");
        }
        auto user_id = (*user_doc)["_id"].get_oid().value;

        //Verifico se il gioco è già stato recensito dall'utente
        auto query = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("$and",
                bsoncxx::builder::basic::make_array(
                    bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("reviews.game_title", game_title)),
                    bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username))
                )
            )
        );

        auto result = userCollection.find_one(query.view());

        if (result) {
            throw CreateReviewException("Il gioco e' gia' stato recensito da questo utente");
        }

        bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> review_doc = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("username", username),
            bsoncxx::builder::basic::kvp("review_text", review_text),
            bsoncxx::builder::basic::kvp("rating", rating),
            bsoncxx::builder::basic::kvp("created_at", bsoncxx::types::b_date(std::chrono::system_clock::now()))
        );

        gameCollection.update_one(
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", game_id)),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$push", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("reviews", review_doc))))
        );

        userCollection.update_one(
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", user_id)),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$push", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("reviews", bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("game_title", game_title),
                bsoncxx::builder::basic::kvp("review_text", review_text),
                bsoncxx::builder::basic::kvp("rating", rating),
                bsoncxx::builder::basic::kvp("created_at", bsoncxx::types::b_date(std::chrono::system_clock::now()))
            )))))
        );
    }
    catch (GetGameException& e) {
        throw;
    }
    catch (UserNotFoundException& e) {
        throw;
    }
    catch (CreateReviewException& e) {
        throw;
    }
    catch (std::exception& e) {
        throw CreateReviewException("Errore durante l'aggiunta della recensione");
    }
}


// Ottenimento di una recensione specifica
nlohmann::json MongoDB::getReview(const std::string& review_id) {
    try {
        auto result = gameCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("reviews._id", bsoncxx::oid(review_id))));
        if (!result) {
            throw ReviewException("Recensione non trovata");
        }

        auto reviews = (*result)["reviews"].get_array().value;
        for (const auto& review : reviews) {
            auto review_doc = review.get_document().value;
            if (review_doc["_id"].get_oid().value.to_string() == review_id) {
                return nlohmann::json::parse(bsoncxx::to_json(review_doc));
            }
        }
        throw ReviewException("Recensione non trovata");
    }
    catch (std::exception& e) {
        throw ReviewException("Errore durante il recupero della recensione");
    }
}

nlohmann::json MongoDB::getReviewByUser(const std::string& username) {
    try {
        auto user_doc = userCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
        if (!user_doc) {
            throw UserNotFoundException("Utente non trovato");
        }
        auto user_id = (*user_doc)["_id"].get_oid().value;

        auto result = gameCollection.find(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("reviews.username", username)));
        nlohmann::json reviews_json;
        for (const auto& doc : result) {
            auto reviews = doc["reviews"].get_array().value;
            for (const auto& review : reviews) {
                auto review_doc = review.get_document().value;
                if (review_doc["username"].get_string().value == username) {
                    reviews_json.push_back(nlohmann::json::parse(bsoncxx::to_json(review_doc)));
                }
            }
        }

        return reviews_json;
    }
    catch (UserNotFoundException& e) {
        throw;
    }
    catch (std::exception& e) {
        throw ReviewException("Errore durante il recupero delle recensioni dell'utente");
    }
}

nlohmann::json MongoDB::getReviewByGame(const std::string& game_title) {
    try {
        auto game_doc = gameCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("title", game_title)));
        if (!game_doc) {
            throw GetGameException("Gioco non trovato");
        }
        auto game_id = (*game_doc)["_id"].get_oid().value;

        auto result = gameCollection.find(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", game_id)));

        nlohmann::json reviews_json;
        for (const auto& doc : result) {
            auto reviews = doc["reviews"].get_array().value;
            for (const auto& review : reviews) {
                auto review_doc = review.get_document().value;
                reviews_json.push_back(nlohmann::json::parse(bsoncxx::to_json(review_doc)));
            }
        }
        return reviews_json;
    }
    catch (GetGameException& e) {
        throw;
    }
    catch (std::exception& e) {
        throw ReviewException("Errore durante il recupero delle recensioni del gioco");
    }
}


// Aggiunta di una prenotazione
void MongoDB::addReservation(const std::string& username, const std::string& game_title, int num_copies) {
    try {
        auto user_doc = userCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
        if (!user_doc) {
            throw UserNotFoundException("User not found");
        }
        auto user_id = (*user_doc)["_id"].get_oid().value;

        auto game_doc = gameCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("title", game_title)));
        if (!game_doc) {
            throw GetGameException("Game not found");
        }

        //Verifico se il gioco è già stato prenotato dall'utente
        auto query = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("$and",
                bsoncxx::builder::basic::make_array(
                    bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("reservations.game_title", game_title)),
                    bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username))
                )
            )
        );

        auto result = userCollection.find_one(query.view());

        if (result) {
            throw CreateReservationException("Il gioco e' gia' stato prenotato da questo utente");
        }

        bsoncxx::document::view game_view = game_doc->view();

        // Prelevo il prezzo del gioco
        auto price_value = game_view["price"];
        if (!price_value) {
            throw CreateReservationException("price not found in game document");
        }

        double price = price_value.get_double().value;

        bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> reservation_doc = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("username", username),
            bsoncxx::builder::basic::kvp("game_title", game_title),
            bsoncxx::builder::basic::kvp("num_copies", num_copies),
            bsoncxx::builder::basic::kvp("price", price),
            bsoncxx::builder::basic::kvp("reservation_date", bsoncxx::types::b_date(std::chrono::system_clock::now()))
        );
        reservationCollection.insert_one(reservation_doc);

        userCollection.update_one(
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$push", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("reservations", reservation_doc))))
        );
    }
    catch (UserNotFoundException& e) {
        throw;
    }
    catch (GetGameException& e) {
        throw;
    }
    catch (CreateReservationException& e) {
        throw;
    }
    catch (std::exception& e) {
        throw CreateReservationException("Errore durante l'aggiunta della prenotazione");
    }
}

void MongoDB::addPurchase(const std::string& username, const std::string& game_title, int num_copies) {
    try {
        auto user_doc = userCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
        if (!user_doc) {
            throw UserNotFoundException("User not found");
        }
        auto user_id = (*user_doc)["_id"].get_oid().value;

        auto game_doc = gameCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("title", game_title)));
        if (!game_doc) {
            throw GetGameException("Game not found");
        }

        bsoncxx::document::view game_view = game_doc->view();

        // Prelevo il prezzo del gioco
        auto price_value = game_view["price"];
        if (!price_value) {
            throw CreateReservationException("price not found in game document");
        }

        double price = price_value.get_double().value;

        bsoncxx::oid purchase_id;

        bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> purchase_doc = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("_id", bsoncxx::oid(purchase_id)),
            bsoncxx::builder::basic::kvp("username", username),
            bsoncxx::builder::basic::kvp("game_title", game_title),
            bsoncxx::builder::basic::kvp("num_copies", num_copies),
            bsoncxx::builder::basic::kvp("price", price),
            bsoncxx::builder::basic::kvp("purchase_date", bsoncxx::types::b_date(std::chrono::system_clock::now()))
        );
        purchaseCollection.insert_one(purchase_doc);

        userCollection.update_one(
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$push", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("purchases", purchase_doc))))
        );
    }
    catch (UserNotFoundException& e) {
        throw;
    }
    catch (GetGameException& e) {
        throw;
    }
    catch (std::exception& e) {
        throw PurchaseException("Error adding purchase");
    }
}

// Ottenimento di una prenotazione specifica
nlohmann::json MongoDB::getReservations(const std::string& username) {
    try {
        auto user_doc = userCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
        if (!user_doc) {
            throw UserNotFoundException("User not found");
        }
        auto user_id = (*user_doc)["_id"].get_oid().value;
        
        auto cursor = reservationCollection.find(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));

        nlohmann::json reservations_json;

        for (const auto& doc : cursor) {
            reservations_json.push_back(nlohmann::json::parse(bsoncxx::to_json(doc)));
        }

        return reservations_json;
    }
    catch (UserNotFoundException& e) {
        throw;
    }
    catch (std::exception& e) {
        throw ReservationException("Errore durante il recupero della prenotazione");
    }
}

nlohmann::json MongoDB::getAllPurchases() {
    try {
        auto cursor = purchaseCollection.find({});

        nlohmann::json purchases_json;

        for (const auto& doc : cursor) {
            purchases_json.push_back(nlohmann::json::parse(bsoncxx::to_json(doc)));
        }

        return purchases_json;
    }
    catch (std::exception& e) {
        throw PurchaseException("Error retrieving all purchases");
    }
}


nlohmann::json MongoDB::getPurchases(const std::string& username) {
    try {
        auto user_doc = userCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
        if (!user_doc) {
            throw UserNotFoundException("User not found");
        }
        auto user_id = (*user_doc)["_id"].get_oid().value;

        auto cursor = purchaseCollection.find(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));

        nlohmann::json purchases_json;

        for (const auto& doc : cursor) {
            purchases_json.push_back(nlohmann::json::parse(bsoncxx::to_json(doc)));
        }

        return purchases_json;
    }
    catch (UserNotFoundException& e) {
        throw;
    }
    catch (std::exception& e) {
        throw PurchaseException("Error retrieving purchases");
    }
}

/*TODO: 
// Ottenimento delle raccomandazioni per un utente
std::vector<bsoncxx::document::value> MongoDB::getRecommendations(const std::string& username) {
    try {
        auto user_doc = userCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
        if (!user_doc) {
            throw UserNotFoundException("User not found");
        }
        auto user_id = (*user_doc)["_id"].get_oid().value;

        std::vector<bsoncxx::document::value> recommendations;
        auto recommendation_doc = recommendationCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("user_id", user_id)));
        if (recommendation_doc) {
            auto recommended_games = (*recommendation_doc)["recommended_games"].get_array().value;
            for (const auto& game : recommended_games) {
                recommendations.push_back(bsoncxx::document::value(game.get_document().value));
            }
        }
        return recommendations;
    }
    catch (std::exception& e) {
        throw GetRecommendationsException("Errore durante il recupero delle raccomandazioni");
    }
}*/

// Funzione helper per convertire stringa a ISODate
std::chrono::system_clock::time_point MongoDB::convertToDate(const std::string& date) {
    std::tm tm = {};
    std::stringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

// Aggiornamento di un utente
void MongoDB::updateUser(const std::string& username, const std::string& new_password, const std::string& new_ImageUrl) noexcept(false) {
    try {
        // Verifico se l'utente esiste
        auto user_doc = userCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
        if (!user_doc) {
            throw UserNotFoundException("Utente non trovato");
        }

        // Costruisco l'aggiornamento
        bsoncxx::builder::basic::document update_builder{};
        if (!new_password.empty()) {
            update_builder.append(bsoncxx::builder::basic::kvp("password", new_password));
        }
        if (!new_ImageUrl.empty()) {
            update_builder.append(bsoncxx::builder::basic::kvp("image_url", new_ImageUrl));
        }

        // Aggiorno il documento dell'utente
        auto result = userCollection.update_one(
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$set", update_builder.extract()))
        );

        if (!result || result->modified_count() == 0) {
            throw HandlerException("Nessuna modifica effettuata per l'utente");
        }
    }
    catch (UserNotFoundException& e) {
        throw;
    }
    catch (const mongocxx::exception& e) {
        throw;
    }
    catch (const std::exception& e) {
        throw;
    }
}

// Aggiornamento di un gioco
void MongoDB::updateGame(const std::string& title, const std::string& newGenre, const std::string& newReleaseDate, const std::string& newDeveloper, double newPrice, int newStock, const std::string& newDescription, const std::string& newImageUrl) noexcept(false) {
    try {
        auto game_doc = gameCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("title", title)));
        if (!game_doc) {
            throw GetGameException("Gioco non trovato");
        }

        bsoncxx::builder::basic::document update_builder{};
        if (!newGenre.empty()) {
            update_builder.append(bsoncxx::builder::basic::kvp("genre", newGenre));
        }
        if (!newReleaseDate.empty()) {
            update_builder.append(bsoncxx::builder::basic::kvp("release_date", bsoncxx::types::b_date(convertToDate(newReleaseDate))));
        }
        if (!newDeveloper.empty()) {
            update_builder.append(bsoncxx::builder::basic::kvp("developer", newDeveloper));
        }
        if (newPrice != -1) {
            update_builder.append(bsoncxx::builder::basic::kvp("price", newPrice));
        }
        if (newStock != -1) {
            update_builder.append(bsoncxx::builder::basic::kvp("stock", newStock));
        }
        if (!newDescription.empty()) {
            update_builder.append(bsoncxx::builder::basic::kvp("description", newDescription));
        }
        if (!newImageUrl.empty()) {
            update_builder.append(bsoncxx::builder::basic::kvp("image_url", newImageUrl));
        }

        auto result = gameCollection.update_one(
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("title", title)),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$set", update_builder.extract()))
        );

        if (!result || result->modified_count() == 0) {
            throw HandlerException("Nessuna modifica effettuata per il gioco");
        }
    }
    catch (GetGameException& e) {
        throw;
    }
    catch (const mongocxx::exception& e) {
        throw;
    }
    catch (const std::exception& e) {
        throw;
    }
}

void MongoDB::updateReview(const std::string& username, const std::string& game_title, const std::string& newReviewText, int newRating) noexcept(false) {
    try {
        // Verifico se il gioco è stato recensito dall'utente
        auto query = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("username", username),
            bsoncxx::builder::basic::kvp("reviews.game_title", game_title)
        );

        auto user_doc = userCollection.find_one(query.view());
        if (!user_doc) {
            throw ReviewException("Recensione non trovata per l'utente");
        }

        // Aggiorno la recensione nel documento dell'utente
        bsoncxx::builder::basic::document update_builder{};
        if (!newReviewText.empty()) {
            update_builder.append(bsoncxx::builder::basic::kvp("reviews.$.review_text", newReviewText));
        }
        if (newRating != -1) {
            update_builder.append(bsoncxx::builder::basic::kvp("reviews.$.rating", newRating));
        }
        
        //update_builder.append(bsoncxx::builder::basic::kvp("reviews.$.review_text", newReviewText));
        //update_builder.append(bsoncxx::builder::basic::kvp("reviews.$.rating", newRating));

        auto result = userCollection.update_one(
            query.view(),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$set", update_builder.extract()))
        );

        if (!result || result->modified_count() == 0) {
            throw ReviewException("Nessuna modifica effettuata per la recensione");
        }

        auto query_game = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("title", game_title),
            bsoncxx::builder::basic::kvp("reviews.username", username)
        );

        // Aggiorno la recensione nel documento del gioco
        bsoncxx::builder::basic::document update_builder_game{};
        if (!newReviewText.empty()) {
            update_builder_game.append(bsoncxx::builder::basic::kvp("reviews.$.review_text", newReviewText));
        }
        if (newRating != -1) {
            update_builder_game.append(bsoncxx::builder::basic::kvp("reviews.$.rating", newRating));
        }

        auto result2 = gameCollection.update_one(
            query_game.view(),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$set", update_builder_game.extract()))
        );

        if (!result2 || result2->modified_count() == 0) {
            throw ReviewException("Nessuna modifica effettuata per la recensione");
        }
    }
    catch (const ReviewException& e) {
        throw;
    }
    catch (const mongocxx::exception& e) {
        throw;
    }
    catch (const std::exception& e) {
        throw;
    }
}

void MongoDB::updateReservation(const std::string& username, const std::string& game_title, int newNumCopies) noexcept(false) {
    try {
        // Verifico se la prenotazione esiste per l'utente e il gioco specificati
        auto query = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("username", username),
            bsoncxx::builder::basic::kvp("reservations.game_title", game_title)
        );

        auto user_doc = userCollection.find_one(query.view());
        if (!user_doc) {
            throw ReservationException("Prenotazione non trovata per l'utente");
        }

        // Aggiorno la prenotazione nel documento dell'utente
        bsoncxx::builder::basic::document update_builder{};
        update_builder.append(bsoncxx::builder::basic::kvp("reservations.$.num_copies", newNumCopies));

        auto result = userCollection.update_one(
            query.view(),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$set", update_builder.extract()))
        );

        if (!result || result->modified_count() == 0) {
            throw ReservationException("Nessuna modifica effettuata per la prenotazione");
        }

        // Aggiorno la prenotazione anche nella collection delle prenotazioni
        auto update_res = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("username", username),
            bsoncxx::builder::basic::kvp("game_title", game_title)
        );

        auto update_result = reservationCollection.update_one(
            update_res.view(),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$set", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("num_copies", newNumCopies))))
        );

        if (!update_result || update_result->modified_count() == 0) {
            throw ReservationException("Nessuna modifica effettuata per la prenotazione nella collection delle prenotazioni");
        }
    }
    catch (const ReservationException& e) {
        throw;
    }
    catch (const mongocxx::exception& e) {
        throw;
    }
    catch (const std::exception& e) {
        throw;
    }
}

void MongoDB::updatePurchase(int newNumCopies, const std::string& purchase_id) noexcept(false) {
    try {
        auto query = bsoncxx::builder::basic::make_document(
            //bsoncxx::builder::basic::kvp("username", username),
            bsoncxx::builder::basic::kvp("purchases._id", bsoncxx::oid(purchase_id))
        );

        auto user_doc = userCollection.find_one(query.view());
        if (!user_doc) {
            throw PurchaseException("Acquisto non trovato per l'utente");
        }

        // Aggiorna la prenotazione nel documento dell'utente
        bsoncxx::builder::basic::document update_builder{};
        update_builder.append(bsoncxx::builder::basic::kvp("purchases.$.num_copies", newNumCopies));

        auto result = userCollection.update_one(
            query.view(),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$set", update_builder.extract()))
        );

        if (!result || result->modified_count() == 0) {
            throw PurchaseException("Nessuna modifica effettuata per l'acquisto");
        }

        // Aggiorna anche la prenotazione nella collection delle prenotazioni
        auto update_res = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("_id", bsoncxx::oid(purchase_id))
        );

        auto update_result = purchaseCollection.update_one(
            update_res.view(),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$set", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("num_copies", newNumCopies))))
        );

        if (!update_result || update_result->modified_count() == 0) {
            throw PurchaseException("Nessuna modifica effettuata per la prenotazione nella collection delle prenotazioni");
        }
    }
    catch (const PurchaseException& e) {
        throw;
    }
    catch (const mongocxx::exception& e) {
        throw;
    }
    catch (const std::exception& e) {
        throw;
    }
}



//TODO: Sistemare funzione
// Aggiornamento di una raccomandazione
/*void MongoDB::updateRecommendation(const std::string& username, const std::vector<std::string>& new_recommendations) {
    try {
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "username" << username;

        bsoncxx::builder::stream::document update_builder;
        update_builder << "$set" << bsoncxx::builder::stream::open_document
            << "recommended_games" << bsoncxx::builder::stream::open_array;
        for (const auto& game_id : new_recommendations) {
            update_builder << bsoncxx::oid::{game_id};
        }
        update_builder << bsoncxx::builder::stream::close_array
            << bsoncxx::builder::stream::close_document;

        auto result = recommendationCollection.update_one(filter_builder.view(), update_builder.view());
        if (result->modified_count() == 0) {
            throw std::runtime_error("Raccomandazioni non aggiornate");
        }
    }
    catch (std::exception& e) {
        throw std::runtime_error("Errore durante l'aggiornamento delle raccomandazioni");
    }
}

*/

void MongoDB::deleteUser(const std::string& username) noexcept(false) {
    try {
        // Costruzione del filtro per l'utente da eliminare
        auto filter = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("username", username)
        );

        // Eliminazione dell'utente dalla collezione userCollection
        auto result = userCollection.delete_one(filter.view());

        if (result) {
            if (result->deleted_count() == 0) {
                // Nessun utente corrispondente è stato trovato
                throw UserNotFoundException("Utente non trovato");
            }
        }
        else {
            throw std::runtime_error("Errore durante l'eliminazione dell'utente");
        }
    }
    catch (const mongocxx::exception& e) {
        throw;
    }
    catch (const std::exception& e) {
        throw;
    }
}

void MongoDB::deleteGame(const std::string& game_title) noexcept(false) {
    try {
        // Costruzione del filtro per il gioco da eliminare
        auto filter = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("title", game_title)
        );

        // Eliminazione del gioco dalla collezione gameCollection
        auto result = gameCollection.delete_one(filter.view());

        if (result) {
            if (result->deleted_count() == 0) {
                // Nessun gioco corrispondente è stato trovato
                throw GetGameException("Gioco non trovato");
            }
        }
        else {
            throw std::runtime_error("Errore durante l'eliminazione del gioco");
        }
    }
    catch (const mongocxx::exception& e) {
        throw;
    }
    catch (const std::exception& e) {
        throw;
    }
}
/*
void MongoDB::deleteReservation(const std::string& reservationId) noexcept(false) {
    try {
        // Costruzione del filtro per la prenotazione da eliminare
        auto filter = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("_id", reservationId)
        );

        // Eliminazione della prenotazione dalla collezione reservationCollection
        auto result = reservationCollection.delete_one(filter.view());

        if (result) {
            if (result->deleted_count() == 0) {
                // Nessuna prenotazione corrispondente è stata trovata
                throw ReservationException("Prenotazione non trovata");
            }
        }
        else {
            throw ReservationException("Errore durante l'eliminazione della prenotazione");
        }

        // Aggiornamento della userCollection per eliminare la prenotazione dall'array "reservations"
        userCollection.update_one(
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("username", username)
            ),
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$pull",
                    bsoncxx::builder::basic::make_document(
                        bsoncxx::builder::basic::kvp("reservations", bsoncxx::builder::basic::make_document(
                            bsoncxx::builder::basic::kvp("game_title", game_title)
                        ))
                    )
                )
            )
        );
    }
    catch (const mongocxx::exception& e) {
        throw;
    }
    catch (const std::exception& e) {
        throw;
    }
}*/

void MongoDB::deleteReservation(const std::string& reservationId) noexcept(false) {
    try {
        // Costruzione del filtro per la prenotazione da eliminare
        auto filter = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("_id", bsoncxx::oid(reservationId))
        );

        // Trova la prenotazione specifica per ottenere username e game_title
        auto reservation = reservationCollection.find_one(filter.view());
        if (!reservation) {
            throw ReservationException("Prenotazione non trovata");
        }

        // Estrazione di username e game_title dalla prenotazione trovata
        auto reservationView = reservation->view();
        std::string username = reservationView["username"].get_utf8().value.to_string();
        std::string game_title = reservationView["game_title"].get_utf8().value.to_string();

        // Eliminazione della prenotazione dalla collezione reservationCollection
        auto result = reservationCollection.delete_one(filter.view());
        if (!result || result->deleted_count() == 0) {
            throw ReservationException("Errore durante l'eliminazione della prenotazione");
        }

        // Aggiornamento della userCollection per eliminare la prenotazione dall'array "reservations"
        userCollection.update_one(
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("username", username)
            ),
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$pull",
                    bsoncxx::builder::basic::make_document(
                        bsoncxx::builder::basic::kvp("reservations",
                            bsoncxx::builder::basic::make_document(
                                bsoncxx::builder::basic::kvp("game_title", game_title)
                            )
                        )
                    )
                )
            )
        );
    }
    catch (const mongocxx::exception& e) {
        throw;
    }
    catch (const std::exception& e) {
        throw;
    }
}

void MongoDB::deletePurchase(const std::string& purchase_id) noexcept(false) {
    try {
        // Costruzione del filtro per il purchase da eliminare
        auto filter = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("_id", bsoncxx::oid(purchase_id))
        );

        // Trova il purchase specifico per ottenere username e game_title
        auto purchase = purchaseCollection.find_one(filter.view());
        if (!purchase) {
            throw PurchaseException("Purchase non trovato");
        }

        // Estrazione di username e game_title dal purchase trovato
        auto purchaseView = purchase->view();
        std::string username = purchaseView["username"].get_utf8().value.to_string();
        std::string game_title = purchaseView["game_title"].get_utf8().value.to_string();

        // Eliminazione del purchase dalla collezione purchaseCollection
        auto result = purchaseCollection.delete_one(filter.view());
        if (!result || result->deleted_count() == 0) {
            throw PurchaseException("Errore durante l'eliminazione del purchase");
        }

        // Aggiornamento della userCollection per eliminare il purchase dall'array "purchases"
        userCollection.update_one(
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("username", username)
            ),
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$pull",
                    bsoncxx::builder::basic::make_document(
                        bsoncxx::builder::basic::kvp("purchases",
                            bsoncxx::builder::basic::make_document(
                                bsoncxx::builder::basic::kvp("_id", bsoncxx::oid(purchase_id))
                            )
                        )
                    )
                )
            )
        );
    }
    catch (const mongocxx::exception& e) {
        throw;
    }
    catch (const std::exception& e) {
        throw;
    }
}


void MongoDB::deleteReview(const std::string& username, const std::string& game_title) noexcept(false) {
    try {

        auto user_doc = userCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
        if (!user_doc) {
            throw UserNotFoundException("Utente non trovato");
        }

        auto game_doc = gameCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("title", game_title)));
        if (!game_doc) {
            throw GetGameException("Gioco non trovato");
        }

        // Costruzione del filtro per la recensione da eliminare nell'array "reviews" del gioco
        auto game_filter = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("title", game_title)
        );

        auto game_update = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("$pull", //per rimuovere l'elemento dall'array
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("reviews",
                        bsoncxx::builder::basic::make_document(
                            bsoncxx::builder::basic::kvp("username", username)
                        )
                    )
                )
            )
        );

        auto game_result = gameCollection.update_one(game_filter.view(), game_update.view());

        if (!game_result || game_result->modified_count() == 0) {
            throw CreateReviewException("Recensione non trovata nel gioco");
        }

        // Aggiornamento della userCollection per eliminare la recensione dall'array "reviews" dell'utente
        auto user_filter = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("username", username)
        );

        auto user_update = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("$pull",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("reviews",
                        bsoncxx::builder::basic::make_document(
                            bsoncxx::builder::basic::kvp("game_title", game_title)
                        )
                    )
                )
            )
        );

        auto user_result = userCollection.update_one(user_filter.view(), user_update.view());

        if (!user_result || user_result->modified_count() == 0) {
            std::cerr << "Recensione non trovata per l'utente" << std::endl;
            throw ReviewException("Recensione non trovata per l'utente");
        }
    }
    catch (GetGameException& e) {
        throw;
    }
    catch (UserNotFoundException& e) {
        throw;
    }
    catch (CreateReviewException& e) {
        throw;
    }
    catch (const mongocxx::exception& e) {
        throw;
    }
    catch (const std::exception& e) {
        throw;
    }
}

