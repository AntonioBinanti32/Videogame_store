#include <cstdlib>
#include "socketTcp/SocketTcp.h"
#include "handler/Handler.h"
#include "INIReader.h"
//TODO: Implementare Exceptions
//TODO: implementare handlers e logica backend

#include <thread>
#include <vector>
#include <filesystem>
using namespace std;

int main() {
    // Recupero file config.ini e relative variabili
    string filePath = __FILE__;
    size_t lastSlashPos = filePath.find_last_of("/\\");
    string path = filePath.substr(0, lastSlashPos);
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

    std::string serverTcpPort = reader.Get("ServerTCP", "Port", "1984");
    const char* serverPort = serverTcpPort.c_str();

    try {
        // Inizializzazione del server
        SocketTcp serverSocket;
        serverSocket.initializeServer(serverPort);
        std::cout << "Server socket initialized on port " << serverPort << std::endl;
        //TODO: Implementare connessione a MongoDB
        serverSocket.connect_to_mongodb(host, port, database, username, password);
        std::cout << "Server connesso al db: " << database << ", host: " << host << ", port: " << port << "\n";
        serverSocket.listenForConnections();
        std::cout << "Listening for connections..." << std::endl;
        std::vector<std::thread> clientThreads;

        while (true) {
            // Accetta una nuova connessione
            SOCKET clientSocket = serverSocket.acceptConnection();
            if (clientSocket != INVALID_SOCKET) {
                std::cout << "New client connected." << std::endl;

                // Crea un thread separato per gestire il client
                //TODO: Implementare logica dell'handler
                //TODO: Implementare le exceptions dell'handler
                clientThreads.emplace_back(handler::handleClient, std::ref(serverSocket), clientSocket);
            }
            else {
                throw SocketException("Failed to accept client connection.");
            }
        }

        // Detach dei thread per gestire clienti concorrenzialmente
        for (auto& th : clientThreads) {
            if (th.joinable()) {
                th.detach();
            }
        }

        // Attesa della connessione di 100 secondi prima di chiudere i threads
        std::this_thread::sleep_for(std::chrono::seconds(100));
    }
    catch (const exception& e) {
        cout << e.what() << endl;
    }

    /*try {
        SocketTCPServer server(8080);
        struct sockaddr_in client_address;
        int client_sock = server.acceptConnection(client_address);
        // Gestisci la connessione del client
        closesocket(client_sock);
    } catch (const SocketException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }*/

    return EXIT_SUCCESS;
}
