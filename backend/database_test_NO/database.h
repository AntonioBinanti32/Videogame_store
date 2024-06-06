#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

class Database {
private:
    connection conn;
public:
    Database(string dbname, string user, string password, string host, string port);
    void insertData(string table, string values);
    void readData(string table);
    void updateData(string table, string set, string condition);
    void deleteData(string table, string condition);
};

#endif /* DATABASE_H */
