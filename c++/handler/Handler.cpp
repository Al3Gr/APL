#include "Handler.h"
#include "cpp-base64/base64.cpp"

namespace apl::handler{

    // gestisce le richieste POST di registrazione al servizio
    void signup_handler(const std::shared_ptr<restbed::Session> session) {
        cout << "signup" << endl;
        const auto request = session->get_request();
        size_t content_length = request->get_header( "Content-Length", 0 );
        session->fetch( content_length, [ request ]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            // analizzo il payload associato alla richiesta POST e lo converto in un JSON
            nlohmann::json requestJson = nlohmann::json::parse(body.data());
            std::string username = requestJson["username"];
            std::string password = requestJson["password"];
            try{
                // Registro l'utente sul database
                MongoDB *mongoDb = MongoDB::getInstance();
                mongoDb->signup(username, password);
                // restituisco all'utente un jwt token che verrà usato per autenticare l'utente per le varie richieste di upload, get e like
                jwt::jwt_object obj{jwt::params::algorithm("HS256"), jwt::params::payload({{"username", username}}), jwt::params::secret("secret")};
                obj.add_claim("exp", std::chrono::system_clock::now() + std::chrono::hours(2));
                session->close( restbed::OK, obj.signature(), { { "Content-Length", to_string(obj.signature().length())}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            } catch (SignupException& e){
                session->close( restbed::BAD_REQUEST, e.what(), { { "Content-Length", to_string(strlen(e.what()))}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            }

        } );
    }

    // gestisce le richieste POST di login al servizio
    void login_handler(const std::shared_ptr<restbed::Session> session) {
        cout << "login" << endl;
        const auto request = session->get_request();
        size_t content_length = request->get_header( "Content-Length", 0 );
        session->fetch( content_length, [ request ]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            // analizzo il payload associato alla richiesta POST e lo converto in un JSON
            nlohmann::json requestJson = nlohmann::json::parse(body.data());
            std::string username = requestJson["username"];
            std::string password = requestJson["password"];
            try{
                // controllo se l'utente è presente nel database
                MongoDB *mongoDb = MongoDB::getInstance();
                mongoDb->login(username, password);
                // restituisco all'utente un jwt token che verrà usato per autenticare l'utente per le varie richieste di upload, get e like
                jwt::jwt_object obj{jwt::params::algorithm("HS256"), jwt::params::payload({{"username", username}}), jwt::params::secret("secret")};
                obj.add_claim("exp", std::chrono::system_clock::now() + std::chrono::hours(2));
                session->close( restbed::OK, obj.signature(), { { "Content-Length", to_string(obj.signature().length())}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            } catch (LoginException& e){
                session->close( restbed::BAD_REQUEST, "User non presente", { { "Content-Length", "17"}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            }

        } );
    }

    // gestisce le richieste POST di upload dell'immagine
    void upload_image_handler(const std::shared_ptr< restbed::Session > session){
        cout << "upload" << endl;
        std::string username;
        const auto request = session->get_request( );
        size_t content_length = request->get_header( "Content-Length", 0 );
        auto token = request->get_header("Authorization", "");
        try {
            // verifico che il token passato nel campo Authorization del header sia corretto; se così non fosse restituisco un errore al client
            auto dec_token = jwt::decode(token, jwt::params::algorithms({"HS256"}), jwt::params::secret("secret"));
            username = dec_token.payload().get_claim_value<std::string>("username");
        } catch (const jwt::TokenExpiredError& e) {
            session->close( restbed::UNAUTHORIZED, e.what(), { { "Content-Length", to_string(strlen(e.what()))}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            return;
        }

        session->fetch( content_length, [ request, username ]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            const char delimiter[2] = "-";
            char *token_tag;
            std::list<std::string> tags;
            char buffer[1024];
            SocketTCP *socket;
            std::hash<std::string> hasher;
            // analizzo il payload associato alla richiesta POST e lo converto in un JSON
            nlohmann::json requestJson = nlohmann::json::parse(body.data());
            std::string description = requestJson["description"];
            std::string image = requestJson["image"];
            int img_lenght = image.length();
            // scrivo l'immagine su RAM
            ofstream MyFile("file.jpeg");
            MyFile << base64_decode(image, false);
            MyFile.close();
            try {
                // trasmetto l'immagine alla rete neurale tramite socket e salvo nella variabile buffer i tag individuati da essa
                socket = new SocketTCP("127.0.0.1", 10001);
                socket->socketConnect();
                socket->socketSend(&img_lenght, sizeof(img_lenght));
                socket->socketSend(image.c_str(), img_lenght);
                socket->socketRecv(buffer, 1024);
                delete socket;
            } catch(SocketException& e){
                delete socket;
                session->close( restbed::INTERNAL_SERVER_ERROR, e.what(), { { "Content-Length", to_string(strlen(e.what()))}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
                return ;
            }
            // converto la stringa di tag in un array di tag che verrà passata alla funzione di upload della classe MongoDB
            token_tag = strtok(buffer, delimiter);
            while(token_tag != nullptr){
                tags.emplace_front(token_tag);
                token_tag = strtok(nullptr, delimiter);
            }
            // ottengo l'istanza del Bucket Minio e salvo l'immagine scritta in RAM all'interno del bucket
            MinIOUploader *minio = MinIOUploader::getInstance();
            std::string key = to_string(hasher(username+image))+".jpeg";
            minio->putImage(Aws::String(key), "file.jpeg");
            // Carico l'immagine nel database e restituisco codice 200 all'utente per dirgli che l'upload è andato bene
            MongoDB *mongoDb = MongoDB::getInstance();
            mongoDb->uploadImage(username, description, "http://localhost:9000/apl/"+key, tags);
            session->close( restbed::OK, "Uploaded", { { "Content-Length", "8"}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
        } );
    }

    // gestisce le richieste GET di ottenimento delle immagini
    void get_image_handler(const std::shared_ptr< restbed::Session > session){
        cout << "get" << endl;
        const auto request = session->get_request();
        auto token = request->get_header("Authorization", "");
        try {
            // verifico che il token passato nel campo Authorization del header sia corretto; se così non fosse restituisco un errore al client
            auto dec_token = jwt::decode(token, jwt::params::algorithms({"HS256"}), jwt::params::secret("secret"));
        } catch (const jwt::TokenExpiredError& e) {
            session->close( restbed::UNAUTHORIZED, e.what(), { { "Content-Length", to_string(strlen(e.what()))}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            return;
        }

        // ottengo i parametri di query qualora fossero presenti
        std::string username = request->get_query_parameter("username", ""); //non uso il token per avere l'username perché voglio specificare se mostrare o meno i propri post (è una get non serve fare autorizzazione forte)
        std::string tag = request->get_query_parameter("tag", "");
        int skip = std::stoi(request->get_query_parameter("skip", "0"));
        bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> query;
        // in base ai parametri presenti costruisco la query da passare successivamente a mongoDB per ottenere le immagini desiderate
        if(!username.empty()){
            query = bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("username", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$ne", username)))
            );
        }
        if(!tag.empty()){
            query = bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("tags", tag)
            );
        }
        if(!username.empty() && !tag.empty()){
            query = bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("username", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$ne", username))),
                    bsoncxx::builder::basic::kvp("tags", tag)
            );
        }

        // passo la query a mongodb e i risultati restituiti vengono passati al client
        MongoDB *mongoDb = MongoDB::getInstance();
        std::string result = mongoDb->getImages(query, skip);

        session->close( restbed::OK, result, { { "Content-Length", to_string(result.length()) }, {"Content-Type", "text/html"}, { "Connection", "close" } } );
    }

    // gestisce le richieste PUT per l'inserimento dei like ai post
    void like_handler(const std::shared_ptr< restbed::Session > session){
        cout << "like" << endl;
        const auto request = session->get_request( );
        size_t content_length = request->get_header( "Content-Length", 0 );
        auto token = request->get_header("Authorization", "");
        std::string username;
        try {
            // verifico che il token passato nel campo Authorization del header sia corretto; se così non fosse restituisco un errore al client
            auto dec_token = jwt::decode(token, jwt::params::algorithms({"HS256"}), jwt::params::secret("secret"));
            username = dec_token.payload().get_claim_value<std::string>("username");
        } catch (const jwt::TokenExpiredError& e) {
            session->close( restbed::UNAUTHORIZED, e.what(), { { "Content-Length", to_string(strlen(e.what()))}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            return;
        }
        try {
        session->fetch( content_length, [ request, username ]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body )
        {
            // analizzo il payload associato alla richiesta PUT e lo converto in un JSON
            nlohmann::json requestJson = nlohmann::json::parse(body.data());
            std::string idImage = requestJson["id"];
            std::string like = requestJson["like"];
            bool likeBoolean = like == "1";
            // contatto il database ed eseguo l'inserimento (se like = 1) o il toglimento (se like = 0) del like dal post indentificato dal campo id
            MongoDB *mongoDb = MongoDB::getInstance();
            if(mongoDb->likeImage(username, idImage, likeBoolean))
                session->close( restbed::OK, "Like", { { "Content-Length", "4"}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
            else
                session->close( restbed::BAD_REQUEST, "Errore", { { "Content-Length", "6"}, {"Content-Type", "text/html"},{ "Connection", "close" } } );
        } );}
        catch (std::exception e){
            cout << e.what();
        }
    }

}

