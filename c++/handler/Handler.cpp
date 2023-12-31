#include "Handler.h"
#include "cpp-base64/base64.cpp"

namespace apl::handler{

    void signup_handler(const std::shared_ptr<restbed::Session> session) {
        cout << "signup" << endl;
        const auto request = session->get_request();
        size_t content_length = request->get_header( "Content-Length", 0 );
        session->fetch( content_length, [ request ]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            nlohmann::json requestJson = nlohmann::json::parse(body.data());
            std::string username = requestJson["username"];
            std::string password = requestJson["password"];
            try{
                MongoDB *mongoDb = MongoDB::getInstance();
                mongoDb->signup(username, password);
                jwt::jwt_object obj{jwt::params::algorithm("HS256"), jwt::params::payload({{"username", username}}), jwt::params::secret("secret")};
                obj.add_claim("exp", std::chrono::system_clock::now() + std::chrono::hours(2));
                session->close( restbed::OK, obj.signature(), { { "Content-Length", to_string(obj.signature().length())}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            } catch (mongocxx::bulk_write_exception& e){
                session->close( restbed::BAD_REQUEST, "User già presente", { { "Content-Length", "18"}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            }

        } );
    }

    void login_handler(const std::shared_ptr<restbed::Session> session) {
        cout << "login" << endl;
        const auto request = session->get_request();
        size_t content_length = request->get_header( "Content-Length", 0 );
        session->fetch( content_length, [ request ]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            nlohmann::json requestJson = nlohmann::json::parse(body.data());
            std::string username = requestJson["username"];
            std::string password = requestJson["password"];
            try{
                MongoDB *mongoDb = MongoDB::getInstance();
                mongoDb->login(username, password);
                jwt::jwt_object obj{jwt::params::algorithm("HS256"), jwt::params::payload({{"username", username}}), jwt::params::secret("secret")};
                obj.add_claim("exp", std::chrono::system_clock::now() + std::chrono::hours(2));
                session->close( restbed::OK, obj.signature(), { { "Content-Length", to_string(obj.signature().length())}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            } catch (LoginException& e){
                session->close( restbed::BAD_REQUEST, "User non presente", { { "Content-Length", "17"}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            }

        } );
    }

    void upload_image_handler(const std::shared_ptr< restbed::Session > session){
        cout << "upload" << endl;
        std::string username;
        const auto request = session->get_request( );
        size_t content_length = request->get_header( "Content-Length", 0 );
        auto token = request->get_header("Authorization", "");
        try {
            auto dec_token = jwt::decode(token, jwt::params::algorithms({"HS256"}), jwt::params::secret("secret"));
            username = dec_token.payload().get_claim_value<std::string>("username");
        } catch (const jwt::TokenExpiredError& e) {
            session->close( restbed::UNAUTHORIZED, e.what(), { { "Content-Length", to_string(strlen(e.what()))}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            return;
        }

        session->fetch( content_length, [ request, username ]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            const char delimiter[2] = "-";
            char *token_tag;
            std::list<std::string> tags;
            char buffer[1024];
            SocketTCP *socket;
            std::hash<std::string> hasher;
            nlohmann::json requestJson = nlohmann::json::parse(body.data());
            std::string description = requestJson["description"];
            std::string image = requestJson["image"];
            int img_lenght = image.length();
            ofstream MyFile("file.jpeg");
            MyFile << base64_decode(image, false);
            MyFile.close();
            try {
                socket = new SocketTCP("127.0.0.1", 10001);
                socket->socketConnect();
                socket->socketSend(&img_lenght, sizeof(img_lenght));
                socket->socketSend(image.c_str(), img_lenght);
                socket->socketRecv(buffer, 1024);
                delete socket;
            } catch(SocketException& e){
                delete socket;
                session->close( restbed::INTERNAL_SERVER_ERROR, e.what(), { { "Content-Length", to_string(strlen(e.what()))}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
                return ;
            }
            token_tag = strtok(buffer, delimiter);
            while(token_tag != nullptr){
                tags.emplace_front(token_tag);
                token_tag = strtok(nullptr, delimiter);
            }
            MinIOUploader *minio = MinIOUploader::getInstance();
            std::string key = to_string(hasher(username+image))+".jpeg";
            minio->putImage(Aws::String(key), "file.jpeg");
            MongoDB *mongoDb = MongoDB::getInstance();
            // TODO: vedere se posso passare tags per riferimento
            mongoDb->uploadImage(username, description, "http://localhost:9000/apl/"+key, tags);
            session->close( restbed::OK, "Uploaded", { { "Content-Length", "8"}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
        } );
    }


    void get_image_handler(const std::shared_ptr< restbed::Session > session){
        cout << "get" << endl;
        const auto request = session->get_request();
        auto token = request->get_header("Authorization", "");
        try {
            auto dec_token = jwt::decode(token, jwt::params::algorithms({"HS256"}), jwt::params::secret("secret"));
        } catch (const jwt::TokenExpiredError& e) {
            session->close( restbed::UNAUTHORIZED, e.what(), { { "Content-Length", to_string(strlen(e.what()))}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            return;
        }
        
        std::string username = request->get_query_parameter("username", ""); //non uso il token per avere l'username perché voglio specificare se mostrare o meno i propri post (è una get non serve fare autorizzazione forte)
        std::string tag = request->get_query_parameter("tag", "");
        int skip = std::stoi(request->get_query_parameter("skip", "0"));
        bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> query;
        if(!username.empty()){
            query = bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("username", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$ne", username)))
            );
        }
        if(!tag.empty()){
            query = bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("tags", tag)
            );
        }
        if(!username.empty() && !tag.empty()){
            query = bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("username", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$ne", username))),
                    bsoncxx::builder::basic::kvp("tags", tag)
            );
        }

        MongoDB *mongoDb = MongoDB::getInstance();
        std::string result = mongoDb->getImages(query, skip);

        session->close( restbed::OK, result, { { "Content-Length", to_string(result.length()) }, {"Content-Type", "text/html"}, { "Connection", "close" } } );
    }

    void like_handler(const std::shared_ptr< restbed::Session > session){
        cout << "like" << endl;
        const auto request = session->get_request( );
        size_t content_length = request->get_header( "Content-Length", 0 );
        auto token = request->get_header("Authorization", "");
        std::string username;
        try {
            auto dec_token = jwt::decode(token, jwt::params::algorithms({"HS256"}), jwt::params::secret("secret"));
            username = dec_token.payload().get_claim_value<std::string>("username");
        } catch (const jwt::TokenExpiredError& e) {
            session->close( restbed::UNAUTHORIZED, e.what(), { { "Content-Length", to_string(strlen(e.what()))}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            return;
        }

        session->fetch( content_length, [ request, username ]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            nlohmann::json requestJson = nlohmann::json::parse(body.data());
            std::string idImage = requestJson["id"];
            std::string like = requestJson["like"];
            bool likeBoolean = like == "true";

            MongoDB *mongoDb = MongoDB::getInstance();
            if(mongoDb->likeImage(username, idImage, likeBoolean))
                session->close( restbed::OK, "Like", { { "Content-Length", "4"}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            else
                session->close( restbed::BAD_REQUEST, "Errore", { { "Content-Length", "6"}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
        } );
    }
}

