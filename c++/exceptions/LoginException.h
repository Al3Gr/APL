//
// Created by Alessandro Gravagno on 20/12/23.
//

#ifndef PROGETTO_LOGINEXCEPTION_H
#define PROGETTO_LOGINEXCEPTION_H

#include "iostream"
#include "exception"

class LoginException: public std::exception{
private:
    const char* cause;
public:
    LoginException(const char* cause){
        this->cause = cause;
    };
    const char* what() const noexcept(true){
        return this->cause;
    }

};

#endif //PROGETTO_LOGINEXCEPTION_H
