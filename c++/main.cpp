#include <cstdlib>
#include "server/Server.h"
#include "handler/Handler.h"
#include "INIReader.h"

int main(){
    INIReader reader("../config/config.ini");
    // leggo il file di configurazione per il database
    std::string host = reader.Get("MongoDB", "Hostname", "");
    std::string port = reader.Get("MongoDB", "Port", "");
    std::string database = reader.Get("MongoDB", "Database", "");
    std::string username = reader.Get("MongoDB", "Username", "");
    std::string password = reader.Get("MongoDB", "Password", "");

    // leggo la porta su cui si mette in ascolto il server
    int server_port = reader.GetInteger("Server", "Port", 1984);

    // leggo il file di configurazione per Minio
    std::string host_minio = reader.Get("Minio", "Hostname", "");
    std::string port_minio = reader.Get("Minio", "Port", "");
    std::string keyId = reader.Get("Minio", "KeyId", "");
    std::string keySecret = reader.Get("Minio", "KeySecret", "");
    std::string bucketName = reader.Get("Minio", "BucketName", "");

    // inizializzo ed avvio il server
    Server server;
    server.set_server_port(server_port);
    server.set_server_threading(std::thread::hardware_concurrency());
    server.connect_to_mongodb(host, port, database, username, password);
    server.connect_to_minio(host_minio, port_minio, keyId, keySecret, bucketName);
    server.add_entrypoint("/signup", "POST", apl::handler::signup_handler);
    server.add_entrypoint("/login", "POST", apl::handler::login_handler);
    server.add_entrypoint("/upload", "POST", apl::handler::upload_image_handler);
    server.start();

    return EXIT_SUCCESS;
}