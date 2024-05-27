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
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

// Registrazione dell'utente (viene inserito un nuovo documento nellla collezione user).
// La funzione può lanciare un'eccezione di tipo diverso da std::bad_alloc (indicato dal noexcept(false)).
// Se è già presente un utente con quell'username, viene lanciata un eccezione
void MongoDB::signup(const std::string& username, const std::string& pwd) noexcept(false) {
    // Creazione nuovo documento di BSON
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("username", username),
        bsoncxx::builder::basic::kvp("password", pwd)
    );
    try {
        // Inserimento documento creato precedentemente nella collezione userCollection
        userCollection.insert_one(doc_value);
    }
    catch (mongocxx::bulk_write_exception& e) {
        //TODO: Sollevare eccezione
        //throw new SignupException("Utente già presente");
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
        // TODO: Sollevare eccezione
        // throw LoginException("User non presente");
    }
}

//TODO: Completare funzioni per gestione db