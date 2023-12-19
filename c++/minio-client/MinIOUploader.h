#ifndef MINIO_CLIENT_H
#define MINIO_CLIENT_H

#include <iostream>
#include <curl/curl.h>
#include <vector>

class MinIOUploader {
public:
    MinIOUploader(std::string minioEndpoint, std::string bucketName);

    ~MinIOUploader();

    void uploadImage(const std::basic_string<char>& objectName, std::vector<unsigned char> &imageData) const;

private:
    static size_t readCallback(void *ptr, size_t size, size_t nmemb, void *userdata);

    CURL *curl;
    std::basic_string<char> minioEndpoint;
    std::basic_string<char> bucketName;

};


#endif MINIO_CLIENT_H
