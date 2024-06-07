#include "MongoDB.h"

//TODO: Installare MongoDB: https://www.youtube.com/watch?v=gB6WLkSrtJk

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
        std::cerr << "Utente già registrato " << e.what() << std::endl;
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
void MongoDB::addGame(const std::string& title, const std::string& genre, const std::string& release_date, const std::string& developer, double price, int stock, const std::string& description, const std::string& imageUrl) { //TODO: Metter immagine faacoltativa gioco
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

// Ottenimento della lista dei giochi
std::vector<bsoncxx::document::value> MongoDB::getGames() {
    std::vector<bsoncxx::document::value> games;
    try {
        auto cursor = gameCollection.find({});
        for (auto&& doc : cursor) {
            games.push_back(bsoncxx::document::value(doc));
        }
    }
    catch (std::exception& e) {
        throw  GetGameException("Errore durante il recupero dei giochi");
    }
    return games;
}

// Ottenimento di un gioco specifico
bsoncxx::document::value MongoDB::getGame(const std::string& game_id) {
    try {
        auto result = gameCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", bsoncxx::oid(game_id))));
        if (!result) {
            throw std::runtime_error("Gioco non trovato");
        }
        return bsoncxx::document::value(*result);
    }
    catch (std::exception& e) {
        throw GetGameException("Errore durante il recupero del gioco");
    }
}

// Aggiunta di una recensione
void MongoDB::addReview(const std::string& username, const std::string& game_id, const std::string& review_text, int rating) {
    try {
        auto user_doc = userCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
        if (!user_doc) {
            throw UserNotFoundException("User not found");
        }
        auto user_id = (*user_doc)["_id"].get_oid().value;

        bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> review_doc = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id),
            bsoncxx::builder::basic::kvp("review_text", review_text),
            bsoncxx::builder::basic::kvp("rating", rating),
            bsoncxx::builder::basic::kvp("created_at", bsoncxx::types::b_date(std::chrono::system_clock::now()))
        );

        gameCollection.update_one(
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", bsoncxx::oid(game_id))),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$push", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("reviews", review_doc))))
        );

        userCollection.update_one(
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", user_id)),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$push", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("reviews", bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("game_id", bsoncxx::oid(game_id)),
                bsoncxx::builder::basic::kvp("review_text", review_text),
                bsoncxx::builder::basic::kvp("rating", rating),
                bsoncxx::builder::basic::kvp("created_at", bsoncxx::types::b_date(std::chrono::system_clock::now()))
            )))))
        );
    }
    catch (std::exception& e) {
        throw CreateReviewException("Errore durante l'aggiunta della recensione");
    }
}

// Ottenimento di una recensione specifica
bsoncxx::document::value MongoDB::getReview(const std::string& review_id) {
    try {
        auto result = gameCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("reviews._id", bsoncxx::oid(review_id))));
        if (!result) {
            throw std::runtime_error("Recensione non trovata");
        }
        auto reviews = (*result)["reviews"].get_array().value;
        for (const auto& review : reviews) {
            auto review_doc = review.get_document().value;
            if (review_doc["_id"].get_oid().value.to_string() == review_id) {
                return bsoncxx::document::value(review_doc);
            }
        }
        throw std::runtime_error("Recensione non trovata");
    }
    catch (std::exception& e) {
        throw std::runtime_error("Errore durante il recupero della recensione");
    }
}

// Aggiunta di una prenotazione
void MongoDB::addReservation(const std::string& username, const std::string& game_id) {
    try {
        auto user_doc = userCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
        if (!user_doc) {
            throw UserNotFoundException("User not found");
        }
        auto user_id = (*user_doc)["_id"].get_oid().value;

        bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> reservation_doc = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("user_id", user_id),
            bsoncxx::builder::basic::kvp("game_id", bsoncxx::oid(game_id)),
            bsoncxx::builder::basic::kvp("reservation_date", bsoncxx::types::b_date(std::chrono::system_clock::now()))
        );
        reservationCollection.insert_one(reservation_doc);

        userCollection.update_one(
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", user_id)),
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$push", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("reservations", reservation_doc))))
        );
    }
    catch (std::exception& e) {
        throw CreateReservationException("Errore durante l'aggiunta della prenotazione");
    }
}

// Ottenimento di una prenotazione specifica
bsoncxx::document::value MongoDB::getReservation(const std::string& reservation_id) {
    try {
        auto result = reservationCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", bsoncxx::oid(reservation_id))));
        if (!result) {
            throw std::runtime_error("Prenotazione non trovata");
        }
        return bsoncxx::document::value(*result);
    }
    catch (std::exception& e) {
        throw std::runtime_error("Errore durante il recupero della prenotazione");
    }
}

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
}

// Funzione helper per convertire stringa a ISODate
std::chrono::system_clock::time_point MongoDB::convertToDate(const std::string& date) {
    std::tm tm = {};
    std::stringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

//TODO: Verificare metodi Update
/*
// Aggiornamento di un utente
void MongoDB::updateUser(const std::string& username, const std::string& new_password, const std::string& new_imageUrl) {
    try {
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "username" << username;

        bsoncxx::builder::stream::document update_builder;
        update_builder << "$set" << bsoncxx::builder::stream::open_document;
        if (!new_password.empty()) {
            update_builder << "password" << new_password;
        }
        if (!new_imageUrl.empty()) {
            update_builder << "image_url" << new_imageUrl;
        }
        update_builder << bsoncxx::builder::stream::close_document;

        auto result = userCollection.update_one(filter_builder.view(), update_builder.view());
        if (result->modified_count() == 0) {
            throw std::runtime_error("Utente non aggiornato");
        }
    }
    catch (std::exception& e) {
        throw std::runtime_error("Errore durante l'aggiornamento dell'utente");
    }
}

// Aggiornamento di un gioco
void MongoDB::updateGame(const std::string& game_id, const std::string& new_title, const std::string& new_genre, const std::string& new_release_date, const std::string& new_developer, double new_price, int new_stock, const std::string& new_description, const std::string& new_imageUrl) {
    try {
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "_id" << bsoncxx::oid(game_id);

        bsoncxx::builder::stream::document update_builder;
        update_builder << "$set" << bsoncxx::builder::stream::open_document
            << "title" << new_title
            << "genre" << new_genre
            << "release_date" << bsoncxx::types::b_date(convertToDate(new_release_date))
            << "developer" << new_developer
            << "price" << new_price
            << "stock" << new_stock
            << "description" << new_description
            << "image_url" << new_imageUrl
            << bsoncxx::builder::stream::close_document;

        auto result = gameCollection.update_one(filter_builder.view(), update_builder.view());
        if (result->modified_count() == 0) {
            throw std::runtime_error("Gioco non aggiornato");
        }
    }
    catch (std::exception& e) {
        throw std::runtime_error("Errore durante l'aggiornamento del gioco");
    }
}*/

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

// Aggiornamento di una recensione
void MongoDB::updateReview(const std::string& review_id, const std::string& new_text, int new_rating) {
    try {
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "reviews._id" << bsoncxx::oid(review_id);

        bsoncxx::builder::stream::document update_builder;
        update_builder << "$set" << bsoncxx::builder::stream::open_document
            << "reviews.$.text" << new_text
            << "reviews.$.rating" << new_rating
            << bsoncxx::builder::stream::close_document;

        auto result = gameCollection.update_one(filter_builder.view(), update_builder.view());
        if (result->modified_count() == 0) {
            throw std::runtime_error("Recensione non aggiornata");
        }
    }
    catch (std::exception& e) {
        throw std::runtime_error("Errore durante l'aggiornamento della recensione");
    }
}

// Aggiornamento di una prenotazione
void MongoDB::updateReservation(const std::string& reservation_id, const std::string& new_game_id) {
    try {
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "_id" << bsoncxx::oid(reservation_id);

        bsoncxx::builder::stream::document update_builder;
        update_builder << "$set" << bsoncxx::builder::stream::open_document
            << "game_id" << new_game_id
            << bsoncxx::builder::stream::close_document;

        auto result = reservationCollection.update_one(filter_builder.view(), update_builder.view());
        if (result->modified_count() == 0) {
            throw std::runtime_error("Prenotazione non aggiornata");
        }
    }
    catch (std::exception& e) {
        throw std::runtime_error("Errore durante l'aggiornamento della prenotazione");
    }
}

//TODO: Controllare funzioni per gestione db

// Aggiornamento di un gioco
void MongoDB::updateGame(const std::string& game_id, const std::string& new_title, const std::string& new_genre, const std::string& new_release_date, const std::string& new_developer, double new_price, int new_stock, const std::string& new_description, const std::string& new_imageUrl) {
    try {
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "_id" << bsoncxx::oid(game_id);

        bsoncxx::builder::stream::document update_builder;
        update_builder << "$set" << bsoncxx::builder::stream::open_document;

        if (!new_title.empty()) update_builder << "title" << new_title;
        if (!new_genre.empty()) update_builder << "genre" << new_genre;
        if (!new_release_date.empty()) update_builder << "release_date" << bsoncxx::types::b_date(convertToDate(new_release_date));
        if (!new_developer.empty()) update_builder << "developer" << new_developer;
        if (new_price >= 0) update_builder << "price" << new_price;
        if (new_stock >= 0) update_builder << "stock" << new_stock;
        if (!new_description.empty()) update_builder << "description" << new_description;
        if (!new_imageUrl.empty()) update_builder << "image_url" << new_imageUrl;

        update_builder << bsoncxx::builder::stream::close_document;

        auto result = gameCollection.update_one(filter_builder.view(), update_builder.view());
        if (result && result->modified_count() == 0) {
            throw std::runtime_error("Gioco non aggiornato");
        }
    }
    catch (std::exception& e) {
        throw std::runtime_error("Errore durante l'aggiornamento del gioco");
    }
}

// Aggiornamento di una raccomandazione
void MongoDB::updateRecommendation(const std::string& username, const std::vector<std::string>& new_recommendations) {
    try {
        // Trova il documento dell'utente
        auto user_doc = userCollection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
        if (!user_doc) {
            throw UserNotFoundException("User not found");
        }
        auto user_id = (*user_doc)["_id"].get_oid().value;

        // Crea il filtro per trovare il documento delle raccomandazioni
        bsoncxx::builder::basic::document filter_builder;
        filter_builder.append(bsoncxx::builder::basic::kvp("user_id", user_id));

        // Crea l'array dei nuovi game_id da aggiornare
        bsoncxx::builder::basic::array recommendations_array;
        for (const auto& game_id : new_recommendations) {
            recommendations_array.append(bsoncxx::oid(game_id));
        }

        // Crea il documento di aggiornamento
        bsoncxx::builder::basic::document update_builder;
        update_builder.append(
            bsoncxx::builder::basic::kvp(
                "$set",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("recommended_games", recommendations_array)
                )
            )
        );

        // Esegue l'aggiornamento
        auto result = recommendationCollection.update_one(filter_builder.view(), update_builder.view());
        if (!result || result->modified_count() == 0) {
            throw std::runtime_error("Raccomandazioni non aggiornate");
        }
    }
    catch (std::exception& e) {
        throw std::runtime_error("Errore durante l'aggiornamento delle raccomandazioni: " + std::string(e.what()));
    }
}



// Aggiornamento di una recensione
void MongoDB::updateReview(const std::string& review_id, const std::string& new_text, int new_rating) {
    try {
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "reviews._id" << bsoncxx::oid(review_id);

        bsoncxx::builder::stream::document update_builder;
        update_builder << "$set" << bsoncxx::builder::stream::open_document
            << "reviews.$.review_text" << new_text
            << "reviews.$.rating" << new_rating
            << bsoncxx::builder::stream::close_document;

        auto result = gameCollection.update_one(filter_builder.view(), update_builder.view());
        if (result && result->modified_count() == 0) {
            throw std::runtime_error("Recensione non aggiornata");
        }
    }
    catch (std::exception& e) {
        throw std::runtime_error("Errore durante l'aggiornamento della recensione");
    }
}


// Aggiornamento di una prenotazione
void MongoDB::updateReservation(const std::string& reservation_id, const std::string& new_game_id) {
    try {
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "_id" << bsoncxx::oid(reservation_id);

        bsoncxx::builder::stream::document update_builder;
        update_builder << "$set" << bsoncxx::builder::stream::open_document
            << "game_id" << bsoncxx::oid(new_game_id)
            << bsoncxx::builder::stream::close_document;

        auto result = reservationCollection.update_one(filter_builder.view(), update_builder.view());
        if (result && result->modified_count() == 0) {
            throw std::runtime_error("Prenotazione non aggiornata");
        }
    }
    catch (std::exception& e) {
        throw std::runtime_error("Errore durante l'aggiornamento della prenotazione");
    }
}*/



//TODO: Completare funzioni per gestione db