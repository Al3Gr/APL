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

void MongoDB::connectDB(const std::string &hostname, const std::string& port, const std::string& databaseName, const std::string& username, const std::string& password) {
    auto *uri = new mongocxx::uri("mongodb://"+username+":"+password+"@"+hostname+":"+port);
    auto *client = new mongocxx::client(*uri);
    database = (*client)[databaseName];

    try{
        userCollection = database["users"];
        photosCollection = database["photos"];
    } catch (std::exception& e){
        std::cout << e.what() << std::endl;
    }
}

void MongoDB::signup(const std::string& username, const std::string& pwd) noexcept(false){
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("username", username),
                bsoncxx::builder::basic::kvp("password", pwd)
            );
    try {
         userCollection.insert_one(doc_value);
    } catch (mongocxx::bulk_write_exception& e) {
        throw e;
    }
}

void MongoDB::login(const std::string &username, const std::string &pwd) noexcept(false){
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("username", username),
            bsoncxx::builder::basic::kvp("password", pwd)
    );
    auto find_one_result = userCollection.find_one(doc_value);
    if(!find_one_result){
        throw LoginException("User non presente");
    }
}

void MongoDB::uploadImage(const std::string &username, const std::string &description, const std::string &url,
                          const std::list<std::string> &tags) {
    bsoncxx::builder::basic::array tags_array;
    bsoncxx::builder::basic::array likes;
     for(auto const& t : tags){
        tags_array.append(t);
     }
     auto time = std::chrono::system_clock::now().time_since_epoch().count();
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("username", username),
            bsoncxx::builder::basic::kvp("description", description),
            bsoncxx::builder::basic::kvp("url", url),
            bsoncxx::builder::basic::kvp("tags", tags_array),
            bsoncxx::builder::basic::kvp("likes", likes),
            bsoncxx::builder::basic::kvp("time", time)
    );

    try {
        auto insert_one_result = photosCollection.insert_one(doc_value);
    } catch (mongocxx::exception& e) {
        throw e;
    }
 }

 bool MongoDB::likeImage(const std::string &username, const std::string &idImage, const bool like) {
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> id_filter = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("_id", bsoncxx::oid(idImage))
    );
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> query;
    if(like){
        query = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$addToSet", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("likes", username))));
    }
    else{
        query = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$pull", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("likes", username))));
    }
    auto update_one_result = photosCollection.update_one(id_filter, query);
    if(!update_one_result){
        return false;
    }
    return true;

 }

std::string MongoDB::getImages(bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value>& query, const int &skip) {
     std::string json = "[";
     mongocxx::options::find option;
     bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> ordering = bsoncxx::builder::basic::make_document(
             bsoncxx::builder::basic::kvp("time", -1)
     );
     option.limit(10);
     option.skip(skip);
     option.sort(ordering);
    auto result = photosCollection.find(query, option);
    for(auto r : result){
        std::cout << bsoncxx::to_json(r, bsoncxx::ExtendedJsonMode::k_relaxed);
        json.append(bsoncxx::to_json(r, bsoncxx::ExtendedJsonMode::k_relaxed));
        json.append(",");
    }
    json = json.substr(0, json.size()-1);
    json.append("]");
    return json;
 }


