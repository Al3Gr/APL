#ifndef PROGETTO_SOCKETEXCEPTION_H
#define PROGETTO_SOCKETEXCEPTION_H

#include "iostream"
#include "exception"

#pragma once

using namespace std;

class SocketException: public exception{
private:
    const char* cause;
public:
    SocketException(const char* cause){
        this->cause = cause;
    };
    const char* what() const noexcept(true){
        return this->cause;
    }

};

#endif //PROGETTO_SOCKETEXCEPTION_H
