#include "MinIOUploader.h"

MinIOUploader* MinIOUploader::INSTANCE;

MinIOUploader::MinIOUploader(const Aws::String &endpoint, const Aws::String &keyId, const Aws::String &keySecret, const Aws::String &bucketName) {
    Aws::InitAPI(this->options);
    Aws::Client::ClientConfiguration clientConfig;

    clientConfig.endpointOverride = endpoint;
    clientConfig.scheme = Aws::Http::Scheme::HTTP;

    Aws::Auth::AWSCredentials credentials;
    credentials.SetAWSAccessKeyId(keyId);
    credentials.SetAWSSecretKey(keySecret);
    this->client =std::make_shared<Aws::S3::S3Client>(credentials, clientConfig, Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy(), false);

    createBucket(bucketName);
}

MinIOUploader* MinIOUploader::getInstance(const Aws::String &endpoint, const Aws::String &keyId, const Aws::String &keySecret, const Aws::String &bucketName) {
    if(INSTANCE == nullptr){
        INSTANCE = new MinIOUploader(endpoint, keyId, keySecret, bucketName);
    }
    return  INSTANCE;
}

MinIOUploader::~MinIOUploader(){
    Aws::ShutdownAPI(this->options);
}

bool MinIOUploader::createBucket(const Aws::String &bucketName) {

    Aws::S3::Model::HeadBucketRequest headReq;
    headReq.WithBucket(bucketName);
    auto outcome_head = client->HeadBucket(headReq);
    if(!outcome_head.IsSuccess()){
        Aws::S3::Model::CreateBucketRequest request;
        request.SetBucket(bucketName);

        Aws::S3::Model::CreateBucketOutcome outcome = client->CreateBucket(request);
        if (!outcome.IsSuccess()) {
            auto err = outcome.GetError();
            std::cerr << "Error: CreateBucket: " <<
                      err.GetExceptionName() << ": " << err.GetMessage() << std::endl;
        }
        else {
            std::cout << "Created bucket " << bucketName << std::endl;
        }

        return outcome.IsSuccess();
    } else {
        std::cout << "bucket already created" << std::endl;
        return true;
    }

}

bool MinIOUploader::putImage(const Aws::String &bucketName, const Aws::String &filename) {
    Aws::S3::Model::PutObjectRequest request;
    request.SetBucket(bucketName);
    request.SetKey(filename);

    //???
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
                  << bucketName << "'.";
    }

    return outcome.IsSuccess();
}

bool MinIOUploader::getImage(const Aws::String &bucketName, const Aws::String &objectKey) {
    Aws::S3::Model::GetObjectRequest request;
    request.SetBucket(bucketName);
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
                  << bucketName << "'." << std::endl;
    }


    return outcome.IsSuccess();
}