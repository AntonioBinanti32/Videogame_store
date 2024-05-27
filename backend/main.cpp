#include <cstdlib>
#include "server/Server.h"
#include "handler/Handler.h"
#include "INIReader.h"
//TODO: Implementare Exceptions
//TODO: Implementare Socket se serve comunicare con altri componenti (es con rete neuronale, vedi handler.cpp)
//TODO: Implementare altra ogica backend (Es: suggerimenti e preferenze utente)

using namespace std;


int main() {

    std::cout << "Directory corrente: " << std::filesystem::current_path() << "\n";
    std::cout << "Directory file: " << __FILE__ << "\n";

    //TODO: Sistemare problema per il path del config
    string filePath = __FILE__;
    size_t lastSlashPos = filePath.find_last_of("/\\");
    string path =  filePath.substr(0, lastSlashPos);

    std::cout << "Percorso del file di configurazione: " << path << "\n";

    string config_file = path + "/config/config.ini";

    if (!std::filesystem::exists(config_file)) {
        std::cout << "Il file 'config.ini' non esiste nella directory corrente.\n";
        return 1;
    }
    INIReader reader(config_file);


    //INIReader reader("../config/config.ini");
    //INIReader reader("C:/Users/AntonioBinanti/Documents/Uni/Secondo anno/Advanced Programming Languages/git/Videogame_store/backend/config/config/config.ini");
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
        server.set_server_threading(std::thread::hardware_concurrency());
        // avvio la connessione con il server di mongoDB e il server di Minio
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