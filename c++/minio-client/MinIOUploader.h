#ifndef MINIO_CLIENT_H
#define MINIO_CLIENT_H

#include <iostream>
#include "fstream"
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/HeadBucketRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/core/http/HttpClient.h>

class MinIOUploader {
private:
    Aws::SDKOptions options;
    std::shared_ptr<Aws::S3::S3Client> client;

    static MinIOUploader* INSTANCE;

    bool createBucket(const Aws::String &bucketName);
    MinIOUploader(const Aws::String &endpoint, const Aws::String &keyId, const Aws::String &keySecret, const Aws::String &bucketName);
public:
    ~MinIOUploader();
    static MinIOUploader* getInstance(const Aws::String &endpoint, const Aws::String &keyId, const Aws::String &keySecret, const Aws::String &bucketName);
    bool putImage(const Aws::String &bucketName, const Aws::String &filename);
    bool getImage(const Aws::String &bucketName, const Aws::String &objectKey);

};


#endif MINIO_CLIENT_H
