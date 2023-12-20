#include <thread>
#include <memory>
#include <cstdlib>
#include <restbed>
#include "database/MongoDB.h"
#include <nlohmann/json.hpp>
#include "SocketTCP.h"

using json = nlohmann::json;

using namespace std;

using namespace restbed;

void get_method_handler( const shared_ptr< Session > session )
{
    std::this_thread::sleep_for (chrono::seconds(5));
    // Session rappresenta una conversazione tra il client e il servizio
    session->close( OK, "Hello, World!", { { "Content-Length", "13" }, { "Connection", "close" } } );
}

void get_name_handler(const shared_ptr< Session >  session){
    const auto req = session->get_request();
    const auto args = req->get_query_parameter("namem", "Pippo");
    const std::string size_argument = to_string(args.length());
    session->close( OK, args, { { "Content-Length", size_argument }, { "Connection", "close" } } );
}

void signup_method_handler(const shared_ptr< Session > session){
    //MongoDB::initDB();
    //MongoDB::connect("localhost", 8081, "apl", "users");
    //MongoDB::signup("Alessandro", "Alessandro");
    const auto request = session->get_request();

    size_t content_length = request->get_header( "Content-Length", 0 );
    session->fetch( content_length, [ request ]( const shared_ptr< Session > session, const Bytes & body )
    {

        json requestJson = json::parse(body.data());
        std::cout << requestJson["username"];

    } );


}


void upload_method_handler( const shared_ptr< Session > session )
{
    const auto request = session->get_request( );

    size_t content_length = 0;
    content_length = request->get_header( "Content-Length", content_length );

    fprintf( stderr, "Content Length: %zu.\n", content_length );

    session->fetch( content_length, [ request ]( const shared_ptr< Session > session, const Bytes & body )
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
            //socket.~SocketTCP();
        } catch(SocketException& e){
            fprintf(stderr, "%s\n", e.what());
        }

        session->close( OK, buffer, { { "Content-Length", to_string(strlen(buffer))}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
    } );
}

void service_ready_handler( Service& )
{
    fprintf( stderr, "Hey! The server is up and running.\n" );
}

int main( const int, const char** )
{

    // Resource: rappresenta un endpoint di comunicazione
    auto resource = make_shared< Resource >( );
    resource->set_path( "/resource" );
    resource->set_method_handler( "GET", get_method_handler );

    // Endpoint POST per il caricamento delle immagini
    auto signup = make_shared<Resource>();
    signup->set_path("/signup");
    signup->set_method_handler("POST", signup_method_handler);

    // Endpoint POST per il caricamento delle immagini
    auto upload = make_shared<Resource>();
    upload->set_path("/upload");
    upload->set_method_handler("POST", upload_method_handler);

    // Settings: rappresenta la configurazione del servizio
    auto settings = make_shared< Settings >( );
    settings->set_port( 1984 );// setto la porta su quale il server è in ascolto
    settings->set_worker_limit(std::thread::hardware_concurrency()); // imposto il numero di thread con cui servire le connessioni in ingresso
    // Returns the number of concurrent threads supported by the implementation.

    auto service = make_shared< Service >( );
    // Publish a RESTful resource for public consumption;
    service->publish( upload );
    service->publish(signup);
    // Set a handler to be invoked once the service is up and ready to serve incoming HTTP requests.
    // viene invocato l'handler quando il servizio è up
    service->set_ready_handler( service_ready_handler );
    // avvia il servizio con le impostazioni (settings) fornite
    service->start( settings );



    return EXIT_SUCCESS;
}
