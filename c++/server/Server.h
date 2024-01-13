#ifndef PROGETTO_SERVER_H
#define PROGETTO_SERVER_H

#include <restbed>
#include "../database/MongoDB.h"
#include "../minio-client/MinIOUploader.h"
#include "../exceptions/MinioException.h"

using namespace restbed;

class Server {
private:
    std::shared_ptr<Service> service;
    std::shared_ptr<Settings> settings;
public:
    Server();
    void set_server_port(const unsigned int& port);
    void set_server_threading(const unsigned int& number_thread);
    void connect_to_mongodb(const std::string& hostname, const std::string& port, const std::string& database, const std::string& username, const std::string& password);
    void connect_to_minio(const std::string& hostname, const std::string& port, const std::string& keyId, const std::string &keySecret, const std::string &bucketName) noexcept(false);
    void add_entrypoint(const std::string& path, const std::string& method, const std::function< void ( const std::shared_ptr< Session > ) >& callback);
    void start();
};


#endif //PROGETTO_SERVER_H
