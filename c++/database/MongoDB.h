#ifndef PROGETTO_MONGODB_H
#define PROGETTO_MONGODB_H

#include "list"
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include "../exceptions/LoginException.h"
#include <nlohmann/json.hpp>


class MongoDB {
private:
    mongocxx::instance *instance;
    mongocxx::database database;
    mongocxx::collection userCollection;
    mongocxx::collection photosCollection;

    static MongoDB* INSTANCE;

    MongoDB();

public:
    static MongoDB* getInstance();
    void connectDB(const std::string& hostname, const std::string& port, const std::string& databaseName, const std::string& username, const std::string& password);
    void signup(const std::string& username, const std::string& pwd) noexcept(false);
    void login(const std::string& username, const std::string& pwd) noexcept(false);
    void uploadImage(const std::string& username, const std::string& description, const std::string& url, const std::list<std::string> tags);
    bool likeImage(const std::string &username, const std::string &idImage, const bool like);
    std::string getImages(bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value>& query, const int& skip);

};


#endif //PROGETTO_MONGODB_H
