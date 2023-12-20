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
    static void connect(std::string& hostname, int port, std::string& database, std::string& collection);
    static void signup(std::string& username, std::string& pwd) noexcept(false);
    static void login(std::string& username, std::string& pwd) noexcept(false);
    void uploadImage();

private:
     static mongocxx::collection coll;

};


#endif //PROGETTO_MONGODB_H
