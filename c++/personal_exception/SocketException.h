#ifndef PROGETTO_SOCKETEXCEPTION_H
#define PROGETTO_SOCKETEXCEPTION_H

#include "iostream"
#include "exception"

using namespace std;

class SocketException: public exception{
private:
    const char* cause;
    char* intro = "Socket Exception";
public:
    SocketException(const char* cause){
        this->cause = cause;
    };
    const char* what() const noexcept{
        strcat(intro, this->cause);
        return intro;
    }

};

#endif //PROGETTO_SOCKETEXCEPTION_H
