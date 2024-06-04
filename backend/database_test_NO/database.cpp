#include "database.h"

Database::Database(string dbname, string user, string password, string host, string port) : conn("dbname=" + dbname + " user=" + user + " password=" + password + " host=" + host + " port=" + port) {
    try {
        if (!conn.is_open()) {
            cerr << "Failed to connect to database" << endl;
            exit(1);
        }
        cout << "Connected to database successfully" << endl;

        // Creazione del database se non esiste
        cout << "Creazione del database..." << endl;
        system(("psql -U " + user + " -c \"CREATE DATABASE IF NOT EXISTS " + dbname + "\"").c_str());

        // Connessione al database appena creato
        conn = connection("dbname=" + dbname + " user=" + user + " password=" + password + " host=" + host + " port=" + port);

        // Creazione della tabella se non esiste
        cout << "Creazione della tabella..." << endl;
        system(("psql -U " + user + " -d " + dbname + " -c \"CREATE TABLE IF NOT EXISTS games (id SERIAL PRIMARY KEY, title VARCHAR(100) NOT NULL, platform VARCHAR(50) NOT NULL, price NUMERIC(8, 2) NOT NULL, release_date DATE NOT NULL)\"").c_str());
    }
    catch (const pqxx::broken_connection& e) {
        cerr << "Errore di connessione al database: " << e.what() << endl;
        exit(1);
    }
}

void Database::insertData(string table, string values) {
    try {
        work txn(conn);
        txn.exec("INSERT INTO " + table + " VALUES (" + values + ")");
        txn.commit();
        cout << "Data inserted successfully" << endl;
    }
    catch (const pqxx::broken_connection& e) {
        cerr << "Errore di connessione al database durante l'inserimento dei dati: " << e.what() << endl;
    }
}

void Database::readData(string table) {
    try {
        nontransaction txn(conn);
        result res = txn.exec("SELECT * FROM " + table);
        cout << "Data from table " + table + ":" << endl;
        for (auto row : res) {
            cout << row[0].c_str() << " | " << row[1].c_str() << " | " << row[2].c_str() << " | " << row[3].c_str() << " | " << row[4].c_str() << endl;
        }
    }
    catch (const pqxx::broken_connection& e) {
        cerr << "Errore di connessione al database durante la lettura dei dati: " << e.what() << endl;
    }
}

void Database::updateData(string table, string set, string condition) {
    try {
        work txn(conn);
        txn.exec("UPDATE " + table + " SET " + set + " WHERE " + condition);
        txn.commit();
        cout << "Data updated successfully" << endl;
    }
    catch (const pqxx::broken_connection& e) {
        cerr << "Errore di connessione al database durante l'aggiornamento dei dati: " << e.what() << endl;
    }
}

void Database::deleteData(string table, string condition) {
    try {
        work txn(conn);
        txn.exec("DELETE FROM " + table + " WHERE " + condition);
        txn.commit();
        cout << "Data deleted successfully" << endl;
    }
    catch (const pqxx::broken_connection& e) {
        cerr << "Errore di connessione al database durante la cancellazione dei dati: " << e.what() << endl;
    }
}
