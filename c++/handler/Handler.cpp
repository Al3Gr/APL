#include "Handler.h"


namespace apl::handler{

    void signup_handler(const std::shared_ptr<restbed::Session> session) {
        MongoDB *mongoDb = MongoDB::getInstance();
        const auto request = session->get_request();
        size_t content_length = request->get_header( "Content-Length", 0 );
        session->fetch( content_length, [ request , &mongoDb]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            mongoDb->setCollection("users");
            nlohmann::json requestJson = nlohmann::json::parse(body.data());
            std::string username = requestJson["username"];
            std::string password = requestJson["password"];
            try{
                mongoDb->signup(username, password);
            } catch (mongocxx::bulk_write_exception& e){
                std::cout << e.what() << std::endl;
            }

        } );
    }

    void login_handler(const std::shared_ptr<restbed::Session> session) {
        MongoDB *mongoDb = MongoDB::getInstance();
        const auto request = session->get_request();
        size_t content_length = request->get_header( "Content-Length", 0 );
        session->fetch( content_length, [ request , &mongoDb]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            mongoDb->setCollection("users");
            nlohmann::json requestJson = nlohmann::json::parse(body.data());
            std::string username = requestJson["username"];
            std::string password = requestJson["password"];
            try{
                mongoDb->login(username, password);
            } catch (LoginException& e){
                std::cout << e.what() << std::endl;
            }

        } );
    }

    void upload_image_handler(const std::shared_ptr< restbed::Session > session){
        const auto request = session->get_request( );
        size_t content_length = request->get_header( "Content-Length", 0 );

        session->fetch( content_length, [ request ]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            const char delimiter[2] = "-";
            char *token;
            std::list<std::string> tags;
            char buffer[1024];
            nlohmann::json requestJson = nlohmann::json::parse(body.data());
            std::string username = requestJson["username"];
            std::string description = requestJson["description"];
            std::string image = requestJson["image"];
            int img_lenght = image.length();
            fprintf(stderr, "%d\n", img_lenght);
            try {
                SocketTCP socket("127.0.0.1", 10001);
                socket.socketConnect();
                socket.socketSend(&img_lenght, sizeof(img_lenght));
                socket.socketSend(image.c_str(), img_lenght);
                socket.socketRecv(buffer, 1024);
            } catch(SocketException& e){
                fprintf(stderr, "%s\n", e.what());
            }
            token = strtok(buffer, delimiter);
            while(token != nullptr){
                tags.emplace_front(token);
                token = strtok(nullptr, delimiter);
            }
            MongoDB *mongoDb = MongoDB::getInstance();
            mongoDb->setCollection("photos");
            mongoDb->uploadImage(username, description, "url", tags);
            session->close( restbed::OK, "Uploaded", { { "Content-Length", "8"}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
        } );
    }
}

