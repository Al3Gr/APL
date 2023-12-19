#include "MinIOUploader.h"

#include <utility>

MinIOUploader::MinIOUploader(std::string minioEndpoint, std::string bucketName) : minioEndpoint(
        std::move(minioEndpoint)), bucketName(std::move(bucketName)) {
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize curl!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

MinIOUploader::~MinIOUploader() {
    if (curl) {
        curl_easy_cleanup(curl);
    }
}


void MinIOUploader::uploadImage(const std::basic_string<char> &objectName,
                                std::vector<unsigned char> &imageData) const {
    // Construct the upload URL
    std::string uploadUrl = std::string(minioEndpoint) + "/" + bucketName + "/" + objectName;

    // Set the upload URL and enable uploading
    curl_easy_setopt(curl, CURLOPT_URL, uploadUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    // Set the callback function for reading data during upload
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, readCallback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &imageData);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(imageData.size()));

    // Perform the upload
    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(result) << std::endl;
        exit(EXIT_FAILURE);
    }
}

size_t MinIOUploader::readCallback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    auto *data = static_cast<std::vector<unsigned char > *>(userdata);
    size_t dataSize = size * nmemb;
    ptrdiff_t bytesToCopy = static_cast<std::vector<int>::difference_type>(std::min(dataSize, data->size()));

    // Copy data to the buffer and remove copied data from the vector
    std::copy(data->begin(), data->begin() + bytesToCopy, static_cast<unsigned char*>(ptr));
    data->erase(data->begin(), data->begin() + bytesToCopy);

    return bytesToCopy;
}
