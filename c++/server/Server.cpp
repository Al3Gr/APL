#include "Server.h"

// creo una nuova istanza del server.
// all'interno del costruttore inizializzo le variabili provate service e settings che serviranno a settare il comportamento
// del server
Server::Server() {
    service = std::make_shared<Service>();
    settings = std::make_shared<Settings>();
}

// imposto la porta su cui si mette in ascolto il database
void Server::set_server_port(const unsigned int &port) {
    settings->set_port(port);
}

// imposto il numero di thread da utilizzare per sfruttare un approccio multi-thread
void Server::set_server_threading(const unsigned int &number_thread) {
    settings->set_worker_limit(number_thread);
}

// mi collego al server di mongoDB specificando host e porta su cui è in ascolto, il database da usare e le credenziali per accedere
void Server::connect_to_mongodb(const std::string &hostname, const std::string &port, const std::string &database, const std::string& username, const std::string& password) {
    MongoDB *mongoDb = MongoDB::getInstance();
    mongoDb->connectDB(hostname, port, database, username, password);
}

// funzione usata per aggiungere in endpoint al server:
// per ogni endpoint specifico il percorso, il metodo HTTP e una funzione di callback da invocare quando ricevo una request sull'endpoint
void Server::add_entrypoint(const std::string &path, const std::string &method,
                            const std::function<void(const std::shared_ptr<Session>)> &callback) {
    auto resource = std::make_shared<Resource>();
    resource->set_path(path);
    resource->set_method_handler(method, callback);
    service->publish(resource);
}

// avvio il server
void Server::start() {
    fprintf(stderr, "SERVER AVVIATO E IN ASCOLTO\n");
    service->start(settings);
}


// mi collego al server di Minio specificando l'host e la porta su cui è in ascolto, le credenziali di accesso e il nome del bucket da utilizzare
void Server::connect_to_minio(const std::string& hostname, const std::string& port, const std::string& keyId, const std::string &keySecret, const std::string &bucketName){
    MinIOUploader *minIoUploader = MinIOUploader::getInstance();
    try {
        minIoUploader->connectToBucket(Aws::String(hostname+":"+port), Aws::String(keyId), Aws::String(keySecret), Aws::String(bucketName));
    } catch (MinioException& e){
        throw e;
    }

}
