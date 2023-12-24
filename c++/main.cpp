#include <cstdlib>
#include "server/Server.h"
#include "handler/Handler.h"
#include "INIReader.h"

int main(){
    // leggo il file di configurazione per il database
    INIReader reader("../config/config.ini");
    std::string host = reader.Get("MongoDB", "Hostname", "");
    std::string port = reader.Get("MongoDB", "Port", "");
    std::string database = reader.Get("MongoDB", "Database", "");
    std::string username = reader.Get("MongoDB", "Username", "");
    std::string password = reader.Get("MongoDB", "Password", "");
    // inizializzo ed avvio il server
    Server server;
    server.set_server_port(1984);
    server.set_server_threading(std::thread::hardware_concurrency());
    server.connect_to_mongodb(host, port, database, username, password);
    server.add_entrypoint("/signup", "POST", apl::handler::signup_handler);
    server.add_entrypoint("/login", "POST", apl::handler::login_handler);
    server.add_entrypoint("/upload", "POST", apl::handler::upload_image_handler);
    server.start();

    return EXIT_SUCCESS;
}