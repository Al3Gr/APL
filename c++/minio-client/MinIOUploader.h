#ifndef MINIO_CLIENT_H
#define MINIO_CLIENT_H

#include <iostream>
#include "fstream"
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/PutBucketPolicyRequest.h>
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
    Aws::String bucketName;

    static MinIOUploader* INSTANCE;
    MinIOUploader();
    bool createBucket();
    bool setBucketPolicy();
public:
    ~MinIOUploader();
    static MinIOUploader* getInstance();
    void connectToBucket(const Aws::String &endpoint, const Aws::String &keyId, const Aws::String &keySecret, const Aws::String &bucketName);
    bool putImage(const Aws::String &key, const Aws::String &filename);

};
#endif //MINIO_CLIENT_H