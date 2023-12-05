#include <thread>
#include <memory>
#include <chrono>
#include <cstdlib>
#include <restbed>
#include <sys/socket.h>
#include <netinet/in.h>
#include "fstream"
#include "iostream"
#include <arpa/inet.h>

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

void get_metodo_handler( const shared_ptr< Session > session )
{
    std::this_thread::sleep_for (chrono::seconds(10));
    session->close( OK, "Hello, Thread!", { { "Content-Length", "13" }, { "Connection", "close" } } );
}

void post_method_handler( const shared_ptr< Session > session )
{
    const auto request = session->get_request( );

    size_t content_length = 0;
    content_length = request->get_header( "Content-Length", content_length );


    session->fetch( content_length, [ request ]( const shared_ptr< Session > session, const Bytes & body )
    {
        FILE* fp = fopen("immagine.jpeg", "wb");
        char buffer[1024];
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = htons(10001);
        address.sin_addr.s_addr = inet_addr("127.0.0.1");
        fwrite(body.data(), 1, body.size(), fp);
        int sockid = socket(AF_INET, SOCK_STREAM, 0);

        if (connect(sockid, (struct sockaddr*)&address, sizeof(struct sockaddr_in) ) < 0) {
            printf("\nConnection Failed \n");
            return -1;
        }
        auto b = body.size();
        send(sockid, (void*)&b, sizeof(b), 0);
        send(sockid, (void*)body.data(), body.size(), 0);
        recv(sockid, buffer, 1024, 0);
        session->close( OK, buffer, { { "Content-Length", to_string(strlen(buffer))}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
    } );
}

void service_ready_handler( Service& )
{
    fprintf( stderr, "Hey! The service is up and running." );
}

int main( const int, const char** )
{
    // Resource: rappresenta un endpoint di comunicazione
    auto resource = make_shared< Resource >( );
    resource->set_path( "/resource" );
    resource->set_method_handler( "GET", get_method_handler );

/*    auto reso = make_shared< Resource >( );
    reso->set_path( "/reso" );
    reso->set_method_handler( "POST", post_method_handler );*/


    auto name = make_shared<Resource>();
    name->set_path("/postm");
    name->set_method_handler("POST", post_method_handler);

    // Settings: rappresenta la configurazione del servizio
    auto settings = make_shared< Settings >( );
    settings->set_port( 1984 );// setto la porta su quale il server è in ascolto
    settings->set_worker_limit(std::thread::hardware_concurrency()); // imposto il numero di thread con cui servire le connessioni in ingresso
    // Returns the number of concurrent threads supported by the implementation.

    auto service = make_shared< Service >( );
    // Publish a RESTful resource for public consumption;
    service->publish( name );
    service->publish(resource);
    // Set a handler to be invoked once the service is up and ready to serve incoming HTTP requests.
    // viene invocato l'handler quando il servizio è up
    service->set_ready_handler( service_ready_handler );
    // avvia il servizio con le impostazioni (settings) fornite
    service->start( settings );

    return EXIT_SUCCESS;
}
