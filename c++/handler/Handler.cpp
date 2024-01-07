#include "Handler.h"


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
                mongoDb->setCollection("users");
                mongoDb->signup(username, password);
                jwt::jwt_object obj{jwt::params::algorithm("HS256"), jwt::params::payload({{"some", "payload"}}), jwt::params::secret("secret")};
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
                mongoDb->setCollection("users");
                mongoDb->login(username, password);
                jwt::jwt_object obj{jwt::params::algorithm("HS256"), jwt::params::payload({{"some", "payload"}}), jwt::params::secret("secret")};
                obj.add_claim("exp", std::chrono::system_clock::now() + std::chrono::hours(2));
                session->close( restbed::OK, obj.signature(), { { "Content-Length", to_string(obj.signature().length())}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            } catch (LoginException& e){
                session->close( restbed::BAD_REQUEST, "User non presente", { { "Content-Length", "17"}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            }

        } );
    }

    void upload_image_handler(const std::shared_ptr< restbed::Session > session){
        cout << "upload" << endl;
        const auto request = session->get_request( );
        size_t content_length = request->get_header( "Content-Length", 0 );
        auto token = request->get_header("Authorization", "");
        try {
            auto dec_token = jwt::decode(token, jwt::params::algorithms({"HS256"}), jwt::params::secret("secret"));
        } catch (const jwt::TokenExpiredError& e) {
            session->close( restbed::UNAUTHORIZED, e.what(), { { "Content-Length", to_string(strlen(e.what()))}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            return;
        }

        session->fetch( content_length, [ request ]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            const char delimiter[2] = "-";
            char *token_tag;
            std::list<std::string> tags;
            char buffer[1024];
            SocketTCP *socket;
            nlohmann::json requestJson = nlohmann::json::parse(body.data());
            std::string username = requestJson["username"];
            std::string description = requestJson["description"];
            std::string image = requestJson["image"];
            ofstream MyFile("filename.txt");
            MyFile << image;
            MyFile.close();
            int img_lenght = image.length();
            fprintf(stderr, "%d\n", img_lenght);
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
            MinIOUploader *minio = MinIOUploader::getInstance("localhost:9000", "username", "password", "apl");
            minio->putImage("apl", "filename.txt");
            MongoDB *mongoDb = MongoDB::getInstance();
            mongoDb->setCollection("photos");
            // TODO: vedere se posso passare tags per riferimento
            mongoDb->uploadImage(username, description, "url", tags);
            session->close( restbed::OK, "Uploaded", { { "Content-Length", "8"}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
        } );
    }
}

