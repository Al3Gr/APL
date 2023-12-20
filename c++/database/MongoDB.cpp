#include "MongoDB.h"

mongocxx::collection MongoDB::coll;


void MongoDB::initDB() {
    auto* instance = new mongocxx::instance();
}

void MongoDB::connect(const std::string &hostname, const int port, const std::string& database, const std::string& collection) {
    //mongocxx::instance instance{};
    auto *uri = new mongocxx::uri("mongodb:/"+hostname+":"+std::to_string(port));
    auto *client = new mongocxx::client(*uri);
    mongocxx::database db = (*client)[database];
    coll = db[collection];
}

void MongoDB::signup(const std::string& username, const std::string& pwd) noexcept(false){
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("username", username),
                bsoncxx::builder::basic::kvp("password", pwd)
            );
    try {
        auto insert_one_result = coll.insert_one(doc_value);
    } catch (mongocxx::bulk_write_exception& e) {
        throw e;
    }
}

void MongoDB::login(const std::string &username, const std::string &pwd) noexcept(false){
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("username", username),
            bsoncxx::builder::basic::kvp("password", pwd)
    );
    auto find_one_result = coll.find_one(doc_value);
    if(!find_one_result){
        throw LoginException("User non presente");
    }
    else {
        std::cout << "TOKEN";
    }
}


