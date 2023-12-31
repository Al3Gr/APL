#include "Server.h"

Server::Server() {
    service = std::make_shared<Service>();
    settings = std::make_shared<Settings>();
}

void Server::set_server_port(const unsigned int &port) {
    settings->set_port(port);
}

void Server::set_server_threading(const unsigned int &number_thread) {
    settings->set_worker_limit(number_thread);
}

void Server::connect_to_mongodb(const std::string &hostname, const std::string &port, const std::string &database, const std::string& username, const std::string& password) {
    MongoDB *mongoDb = MongoDB::getInstance();
    mongoDb->connectDB(hostname, port, database, username, password);
}

void Server::add_entrypoint(const std::string &path, const std::string &method,
                            const std::function<void(const std::shared_ptr<Session>)> &callback) {
    auto resource = std::make_shared<Resource>();
    resource->set_path(path);
    resource->set_method_handler(method, callback);
    service->publish(resource);
}

void Server::start() {
    service->start(settings);
}

void Server::connect_to_minio(const std::string& hostname, const std::string& port, const std::string& keyId, const std::string &keySecret, const std::string &bucketName){
    MinIOUploader *minIoUploader = MinIOUploader::getInstance();
    minIoUploader->connectToBucket(Aws::String(hostname+":"+port), Aws::String(keyId), Aws::String(keySecret), Aws::String(bucketName));
}
