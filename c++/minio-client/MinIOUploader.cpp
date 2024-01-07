#include "MinIOUploader.h"

MinIOUploader* MinIOUploader::INSTANCE;

MinIOUploader::MinIOUploader() {
    Aws::InitAPI(this->options);
}

void MinIOUploader::connectToBucket(const Aws::String &endpoint, const Aws::String &keyId, const Aws::String &keySecret, const Aws::String &bucketName) {
    Aws::Client::ClientConfiguration clientConfig;

    clientConfig.endpointOverride = endpoint;
    clientConfig.scheme = Aws::Http::Scheme::HTTP;

    Aws::Auth::AWSCredentials credentials;
    credentials.SetAWSAccessKeyId(keyId);
    credentials.SetAWSSecretKey(keySecret);
    this->client =std::make_shared<Aws::S3::S3Client>(credentials, clientConfig, Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy(), false);
    this->bucketName = bucketName;
    createBucket();
    setBucketPolicy();
}

MinIOUploader* MinIOUploader::getInstance() {
    if(INSTANCE == nullptr){
        INSTANCE = new MinIOUploader();
    }
    return  INSTANCE;
}

MinIOUploader::~MinIOUploader(){
    Aws::ShutdownAPI(this->options);
}

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

    Aws::S3::Model::PutBucketPolicyRequest request;
    request.SetBucket(this->bucketName);
    request.SetBody(request_body);

    Aws::S3::Model::PutBucketPolicyOutcome outcome = client->PutBucketPolicy(request);

    if (!outcome.IsSuccess()) {
        std::cerr << "Error: PutBucketPolicy: "
                  << outcome.GetError().GetMessage() << std::endl;
    }
    else {
        std::cout << "Set the following policy body for the bucket '" <<
                  this->bucketName << "':" << std::endl << std::endl;
        std::cout << policy << std::endl;
    }

    return outcome.IsSuccess();
}

bool MinIOUploader::createBucket() {

    Aws::S3::Model::HeadBucketRequest headReq;
    headReq.WithBucket(this->bucketName);
    auto outcome_head = client->HeadBucket(headReq);
    if(!outcome_head.IsSuccess()){
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

bool MinIOUploader::putImage(const Aws::String &key, const Aws::String &filename) {
    Aws::S3::Model::PutObjectRequest request;
    request.SetBucket(this->bucketName);
    request.SetKey(key);

    //Aws::FStream is used to upload the contents of the local file to the bucket.
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

bool MinIOUploader::getImage( const Aws::String &objectKey) {
    Aws::S3::Model::GetObjectRequest request;
    request.SetBucket(this->bucketName);
    request.SetKey(objectKey);

    Aws::S3::Model::GetObjectOutcome outcome = client->GetObject(request);
    auto b = outcome.GetResult().GetBody().rdbuf();
    if (!outcome.IsSuccess()) {
        const Aws::S3::S3Error &err = outcome.GetError();
        std::cerr << "Error: GetObject: " <<
                  err.GetExceptionName() << ": " << err.GetMessage() << std::endl;
    }
    else {
        std::cout << "Successfully retrieved '" << objectKey << "' from '"
                  << this->bucketName << "'." << std::endl;
    }


    return outcome.IsSuccess();
}