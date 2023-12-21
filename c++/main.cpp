#include <cstdlib>
#include "server/Server.h"
#include "handler/Handler.h"

int main(){
    Server server;
    server.set_server_port(1984);
    server.set_server_threading(std::thread::hardware_concurrency());
    server.connect_to_mongodb("localhost", "8081", "apl");
    server.add_entrypoint("/signup", "POST", apl::handler::signup_handler);
    server.add_entrypoint("/login", "POST", apl::handler::login_handler);
    server.add_entrypoint("/upload", "POST", apl::handler::upload_image_handler);
    server.start();

    return EXIT_SUCCESS;
}