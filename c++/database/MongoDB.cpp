#include "MongoDB.h"

MongoDB* MongoDB::INSTANCE;

// nel costruttore ottengo un'istanza di mongocxx da usare per comunicare col database
MongoDB::MongoDB() {
    instance = new mongocxx::instance();
}

// implemento il pattern Singleton affiché venga creata una sola istanza della classe MongoDB
// se l'istanza è già stata creata, la restituisco
MongoDB* MongoDB::getInstance() {
    if(INSTANCE == nullptr){
        INSTANCE = new MongoDB();
    }
    return INSTANCE;
}

// effettuo la connessione al database, specificando dove si trova il server, le credenziali di accesso e quale database usare
void MongoDB::connectDB(const std::string &hostname, const std::string& port, const std::string& databaseName, const std::string& username, const std::string& password) {
    auto *uri = new mongocxx::uri("mongodb://"+username+":"+password+"@"+hostname+":"+port);
    auto *client = new mongocxx::client(*uri);
    database = (*client)[databaseName];

    // qui ottengo il riferimento alle due collezioni user e photos con cui andrò a lavorare
    try{
        userCollection = database["users"];
        photosCollection = database["photos"];
    } catch (std::exception& e){
        std::cout << e.what() << std::endl;
    }
}

// funzione usata per svolgere la registrazione dell'utente (viene inserito un nuovo documento nellla collezione user).
// Se è già presente un utente con quell'username, viene lanciata un eccezione
void MongoDB::signup(const std::string& username, const std::string& pwd) noexcept(false){
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("username", username),
                bsoncxx::builder::basic::kvp("password", pwd)
            );
    try {
         userCollection.insert_one(doc_value);
    } catch (mongocxx::bulk_write_exception& e) {
        throw new SignupException("Utente già presente");
    }
}

// funzione usata per svolgere il login dell'utente. Se non è presente nel database lancio un eccezione di Login Exception
void MongoDB::login(const std::string &username, const std::string &pwd) noexcept(false){
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("username", username),
            bsoncxx::builder::basic::kvp("password", pwd)
    );
    auto find_one_result = userCollection.find_one(doc_value);
    if(!find_one_result){
        throw LoginException("User non presente");
    }
}

// funzione usata per l'inserimento di un'immagine nel database: per ogni immagine salvo l'utente che l'ha caricata,
// la descrizione fornita, i tag restituiti dalla rete neurale, l'url di minio dal quale recuperarla, un array che conterrà gli utenti che hanno messo mi piace
// e l'istante di tempo in cui viene caricata la foto
void MongoDB::uploadImage(const std::string &username, const std::string &description, const std::string &url,
                          const std::list<std::string> &tags) {
    bsoncxx::builder::basic::array tags_array;
    bsoncxx::builder::basic::array likes;
     for(auto const& t : tags){
        tags_array.append(t);
     }
     auto time = std::chrono::system_clock::now().time_since_epoch().count();
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> doc_value = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("username", username),
            bsoncxx::builder::basic::kvp("description", description),
            bsoncxx::builder::basic::kvp("url", url),
            bsoncxx::builder::basic::kvp("tags", tags_array),
            bsoncxx::builder::basic::kvp("likes", likes),
            bsoncxx::builder::basic::kvp("time", time)
    );

    try {
        auto insert_one_result = photosCollection.insert_one(doc_value);
    } catch (mongocxx::exception& e) {
        throw UploadException("Errore nel caricamento");
    }
 }

 // registro l'utente che ha messo mi piace nel documento relativo al post piaciuto
 bool MongoDB::likeImage(const std::string &username, const std::string &idImage, const bool like) {
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> id_filter = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("_id", bsoncxx::oid(idImage))
    );
    bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> query;
    if(like){
        // se non aveva messo mi piace in precedenza lo mette
        query = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$addToSet", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("likes", username))));
    }
    else{
        // se lo aveva già messo lo toglie dal field likes
        query = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$pull", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("likes", username))));
    }
    auto update_one_result = photosCollection.update_one(id_filter, query);
    if(!update_one_result){
        return false;
    }
    return true;

 }

 // funzione usata per l'ottenimento di una stringa contenente i documenti (i post) che si desiderano visualizzare nell'applicazione
 // secondo quanto specificato nel parametro query
std::string MongoDB::getImages(bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value>& query, const int &skip) {
     std::string json = "[";
     mongocxx::options::find option;
     bsoncxx::view_or_value<bsoncxx::document::view, bsoncxx::document::value> ordering = bsoncxx::builder::basic::make_document(
             bsoncxx::builder::basic::kvp("time", -1)
     );
     option.limit(10);
     option.skip(skip);
     option.sort(ordering);
    auto result = photosCollection.find(query, option);
    for(auto r : result){
        std::cout << bsoncxx::to_json(r, bsoncxx::ExtendedJsonMode::k_relaxed);
        json.append(bsoncxx::to_json(r, bsoncxx::ExtendedJsonMode::k_relaxed));
        json.append(",");
    }
    json = json.substr(0, json.size()-1);
    json.append("]");
    return json;
 }


