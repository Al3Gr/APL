#ifndef PROGETTO_MINIOEXCEPTION_H
#define PROGETTO_MINIOEXCEPTION_H

#include "iostream"
#include "exception"

#pragma once

using namespace std;

class MinioException: public exception{
private:
    const char* cause;
public:
    MinioException(const char* cause){
        this->cause = cause;
    };
    const char* what() const noexcept(true){
        return this->cause;
    }

};

#endif //PROGETTO_MINIOEXCEPTION_H
