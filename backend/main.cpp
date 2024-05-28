#include <cstdlib>
#include "server/Server.h"
#include "handler/Handler.h"
#include "INIReader.h"
//TODO: Implementare Exceptions
//TODO: Implementare Socket se serve comunicare con altri componenti (es con rete neuronale, vedi handler.cpp)
//TODO: Implementare altra logica backend (Es: suggerimenti e preferenze utente)

using namespace std;


int main() {

    // Recupero file config.ini e relative variabili
    string filePath = __FILE__;
    size_t lastSlashPos = filePath.find_last_of("/\\");
    string path =  filePath.substr(0, lastSlashPos);
    string config_file = path + "/config/config.ini";
    if (!std::filesystem::exists(config_file)) {
        std::cout << "Il file 'config.ini' non esiste nella directory corrente.\n";
        return 1;
    }
    INIReader reader(config_file);
    if (reader.ParseError() < 0) {
        std::cout << "Can't load 'config.ini'\n";
        return 1;
    }

    // File di configurazione per il database
    std::string host = reader.Get("MongoDB", "Hostname", "");
    std::string port = reader.Get("MongoDB", "Port", "");
    std::string database = reader.Get("MongoDB", "Database", "");
    std::string username = reader.Get("MongoDB", "Username", "");
    std::string password = reader.Get("MongoDB", "Password", "");

    // Porta su cui si mette in ascolto il server
    int server_port = reader.GetInteger("Server", "Port", 1984);

    try {
        // Inizializzazione ed avvio del server
        Server server;
        server.set_server_port(server_port);
        //TODO: Vedere se necessario utilizzare threads
        //server.set_server_threading(std::thread::hardware_concurrency());
        // Avvio la connessione con il server di mongoDB
        server.connect_to_mongodb(host, port, database, username, password);
        //TODO: Aggiungere altri entrypoints
        // Setto gli endpoint su cui risponderà il server
        server.add_entrypoint("/signup", "POST", videogame_store::handler::signup_handler);
        server.add_entrypoint("/login", "POST", videogame_store::handler::login_handler);
        server.add_entrypoint("/upload", "POST", videogame_store::handler::upload_handler);
        server.start();
    }
    catch (const exception& e) {
        cout << e.what() << endl;
    }


    return EXIT_SUCCESS;
}