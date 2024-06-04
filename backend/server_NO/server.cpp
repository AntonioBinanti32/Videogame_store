
#include "Server.h"

// Creazione nuova istanza del server.
// All'interno del costruttore inizializzo le variabili provate service e settings che serviranno a settare il comportamento del server
Server::Server() {
    service = std::make_shared<Service>();
    settings = std::make_shared<Settings>();
}

// Porta su cui si mette in ascolto il database
void Server::set_server_port(const unsigned int& port) {
    settings->set_port(port);
}

// Numero di thread da utilizzare per sfruttare un approccio multi-thread
void Server::set_server_threading(const unsigned int& number_thread) {
    settings->set_worker_limit(number_thread);
}

// ollegamento al server di mongoDB specificando host e porta su cui è in ascolto, il database da usare e le credenziali per accedere
void Server::connect_to_mongodb(const std::string& hostname, const std::string& port, const std::string& database, const std::string& username, const std::string& password) {
    MongoDB* mongoDb = MongoDB::getInstance();
    mongoDb->connectDB(hostname, port, database, username, password);
}

// Per aggiungere in endpoint al server:
// Per ogni endpoint specifico il percorso, il metodo HTTP e una funzione di callback da invocare quando ricevo una request sull'endpoint
void Server::add_entrypoint(const std::string& path, const std::string& method,
    const std::function<void(const std::shared_ptr<Session>)>& callback) {
    auto resource = std::make_shared<Resource>();
    resource->set_path(path);
    resource->set_method_handler(method, callback);
    service->publish(resource);
}

// Avvio il server
void Server::start() {
    fprintf(stderr, "SERVER AVVIATO E IN ASCOLTO\n");
    service->start(settings);
}
