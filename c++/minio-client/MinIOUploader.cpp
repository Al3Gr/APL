#include "MinIOUploader.h"

MinIOUploader* MinIOUploader::INSTANCE;

MinIOUploader::MinIOUploader() {
    Aws::InitAPI(this->options);
}

// mi collego al server Minio specificando dove si trova, le credenziali di accesso e il nome del bucket da usare
void MinIOUploader::connectToBucket(const Aws::String &endpoint, const Aws::String &keyId, const Aws::String &keySecret, const Aws::String &bucketName) {
    Aws::Client::ClientConfiguration clientConfig;

    clientConfig.endpointOverride = endpoint;
    clientConfig.scheme = Aws::Http::Scheme::HTTP;

    Aws::Auth::AWSCredentials credentials;
    credentials.SetAWSAccessKeyId(keyId);
    credentials.SetAWSSecretKey(keySecret);
    this->client =std::make_shared<Aws::S3::S3Client>(credentials, clientConfig, Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy(), false);
    this->bucketName = bucketName;
    // creo il bucket
    createBucket();
    // imposto la policy di accesso al bucket
    setBucketPolicy();
}

// Implemento il pattern Singleton in modo da avere una sola istanza della classe MinIOUploader da utilizzare per tutte le interazione
// col bucket
MinIOUploader* MinIOUploader::getInstance() {
    if(INSTANCE == nullptr){
        INSTANCE = new MinIOUploader();
    }
    return  INSTANCE;
}

// dealloco le risorse utilizzate
MinIOUploader::~MinIOUploader(){
    Aws::ShutdownAPI(this->options);
}

// funzione usata per impostare la policy del bucket in modo da poter accedere ai file salvati in modo agevole
// la funzione è privata in quanto la policy del bucket deve essere impostata solo durante la creazione
// dell'istanza di Miniouploader
bool MinIOUploader::setBucketPolicy() {
    Aws::String policy = "{\n"
                         "     \"Version\":\"2012-10-17\",\n"
                         "      \"Statement\":[\n"
                         "          {\n"
                         "             \"Effect\": \"Allow\",\n"
                         "             \"Principal\":{\n"
                         "                  \"AWS\":[\"*\"]\n"
                         "               },\n"
                         "              \"Action\": [\"s3:GetObject\" ],\n"
                         "              \"Resource\": [\"arn:aws:s3:::"
                         + this->bucketName + "/*\"]}]}";

    std::shared_ptr<Aws::StringStream> request_body =
            Aws::MakeShared<Aws::StringStream>("");
    *request_body << policy;

    // Creo la richiesta e la sottopongo al server
    // Controllo il risultato della richiesta per capire se la policy è stata impostata correttamente
    Aws::S3::Model::PutBucketPolicyRequest request;
    request.SetBucket(this->bucketName);
    request.SetBody(request_body);

    Aws::S3::Model::PutBucketPolicyOutcome outcome = client->PutBucketPolicy(request);

    if (!outcome.IsSuccess()) {
        std::cerr << "Error: PutBucketPolicy: "
                  << outcome.GetError().GetMessage() << std::endl;
    }
    else {
        std::cout << "Policy setted for the bucket: " <<
                  this->bucketName << "':" << std::endl;
    }

    return outcome.IsSuccess();
}

// funzione usata per la creazione del bucket
// se esso è già presente non faccio nulla altrimenti lo creo
// la funzione è privata in quanto l'eventuale creazione del bucket deve avvenire solo durante la creazione
// dell'istanza di Miniouploader
bool MinIOUploader::createBucket() {

    // controllo se il bucket che si vuole creare sia già presente o meno
    Aws::S3::Model::HeadBucketRequest headReq;
    headReq.WithBucket(this->bucketName);
    auto outcome_head = client->HeadBucket(headReq);
    if(!outcome_head.IsSuccess()){
        // entro qui dentro se il bucket non è creato e, di conseguenza, lo creo
        Aws::S3::Model::CreateBucketRequest request;
        request.SetBucket(this->bucketName);

        Aws::S3::Model::CreateBucketOutcome outcome = client->CreateBucket(request);
        if (!outcome.IsSuccess()) {
            auto err = outcome.GetError();
            std::cerr << "Error: CreateBucket: " <<
                      err.GetExceptionName() << ": " << err.GetMessage() << std::endl;
        }
        else {
            std::cout << "Created bucket " << this->bucketName << std::endl;
        }

        return outcome.IsSuccess();
    } else {
        std::cout << "bucket already created" << std::endl;
        return true;
    }

}

// funzione usata per caricare l'immagine specificata dal parametro filename
// l'immagine verrà salavata nel bucket con la key specificata
bool MinIOUploader::putImage(const Aws::String &key, const Aws::String &filename) {
    Aws::S3::Model::PutObjectRequest request;
    request.SetBucket(this->bucketName);
    request.SetKey(key);

    //Aws::FStream is used to upload the contents of the local file to the bucket.
    // trasformo l'immagine in un IOStream che passo alla request
    // dopodichè controllo se l'inserimento è andato a buon fine
    std::shared_ptr<Aws::IOStream> inputData =
            Aws::MakeShared<Aws::FStream>("SampleAllocationTag",
                                          filename.c_str(),
                                          std::ios_base::in | std::ios_base::binary);

    if (!*inputData) {
        std::cerr << "Error unable to read file " << filename << std::endl;
        return false;
    }

    request.SetBody(inputData);

    Aws::S3::Model::PutObjectOutcome outcome = client->PutObject(request);

    if (!outcome.IsSuccess()) {
        std::cerr << "Error: PutObject: " <<
                  outcome.GetError().GetMessage() << std::endl;
    }
    else {
        std::cout << "Added object '" << filename << "' to bucket '"
                  << this->bucketName << "'.";
    }

    return outcome.IsSuccess();
}