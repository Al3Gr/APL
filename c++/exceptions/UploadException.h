#ifndef PROGETTO_UPLOADEXCEPTION_H
#define PROGETTO_UPLOADEXCEPTION_H

#include "iostream"
#include "exception"

#pragma once

using namespace std;

class UploadException: public exception{
private:
    const char* cause;
public:
    UploadException(const char* cause){
        this->cause = cause;
    };
    const char* what() const noexcept(true){
        return this->cause;
    }

};
#endif //PROGETTO_UPLOADEXCEPTION_H
