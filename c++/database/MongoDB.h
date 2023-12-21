#ifndef PROGETTO_MONGODB_H
#define PROGETTO_MONGODB_H

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include "../exceptions/LoginException.h"

class MongoDB {
private:
    mongocxx::instance *instance;
    mongocxx::database database;
    mongocxx::collection collection;

    static MongoDB* INSTANCE;

    MongoDB();

public:
    static MongoDB* getInstance();
    void connectDB(const std::string& hostname, const int port, const std::string& databaseName);
    void setCollection(const std::string& collectionName);
    void signup(const std::string& username, const std::string& pwd) noexcept(false);
    void login(const std::string& username, const std::string& pwd) noexcept(false);
    void uploadImage();


};


#endif //PROGETTO_MONGODB_H
