#ifndef PROGETTO_MONGODB_H
#define PROGETTO_MONGODB_H

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include "../exceptions/LoginException.h"

class MongoDB {
public:
    static void initDB();
    static void connect(const std::string& hostname, const int port, const std::string& database, const std::string& collection);
    static void signup(const std::string& username, const std::string& pwd) noexcept(false);
    static void login(const std::string& username, const std::string& pwd) noexcept(false);
    void uploadImage();

private:
     static mongocxx::collection coll;

};


#endif //PROGETTO_MONGODB_H
