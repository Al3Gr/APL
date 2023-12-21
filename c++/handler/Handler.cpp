#include "Handler.h"


namespace apl::handler{

    void signup_handler(const std::shared_ptr<restbed::Session> session) {
        MongoDB *mongoDb = MongoDB::getInstance();
        const auto request = session->get_request();
        size_t content_length = request->get_header( "Content-Length", 0 );
        session->fetch( content_length, [ request , &mongoDb]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            mongoDb->setCollection("users");
            auto a = body.data();
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
            auto a = body.data();
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

        fprintf( stderr, "Content Length: %zu.\n", content_length );

        session->fetch( content_length, [ request ]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            char buffer[1024];
            auto size = body.size();
            auto data = body.data();

            try {
                SocketTCP socket("127.0.0.1", 10001);
                socket.socketConnect();
                socket.socketSend(&size, sizeof(size));
                socket.socketSend(data, size);
                socket.socketRecv(buffer, 1024);
            } catch(SocketException& e){
                fprintf(stderr, "%s\n", e.what());
            }

            session->close( restbed::OK, buffer, { { "Content-Length", std::to_string(strlen(buffer))}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
        } );
    }
}

