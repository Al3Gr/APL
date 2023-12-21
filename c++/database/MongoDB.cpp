#include "MongoDB.h"

MongoDB* MongoDB::INSTANCE;

 MongoDB::MongoDB() {
    instance = new mongocxx::instance();
}

MongoDB* MongoDB::getInstance() {
    if(INSTANCE == nullptr){
        INSTANCE = new MongoDB();
    }
    return INSTANCE;
}

void MongoDB::connectDB(const std::string &hostname, const int port, const std::string& databaseName) {
    auto *uri = new mongocxx::uri("mongodb://"+hostname+":"+std::to_string(port));
    auto *client = new mongocxx::client(*uri);
    database = (*client)[databaseName];
}

void MongoDB::setCollection(const std::string &collectionName) {
    collection = database[collectionName];
}

void MongoDB::signup(const std::string& username, const std::string& pwd) noexcept(false){
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("username", username),
                bsoncxx::builder::basic::kvp("password", pwd)
            );
    try {
        auto insert_one_result = collection.insert_one(doc_value);
    } catch (mongocxx::bulk_write_exception& e) {
        throw e;
    }
}

void MongoDB::login(const std::string &username, const std::string &pwd) noexcept(false){
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("username", username),
            bsoncxx::builder::basic::kvp("password", pwd)
    );
    auto find_one_result = collection.find_one(doc_value);
    if(!find_one_result){
        throw LoginException("User non presente");
    }
    else {
        std::cout << "TOKEN";
    }
}


