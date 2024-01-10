#ifndef PROGETTO_SIGNUPEXCEPTION_H
#define PROGETTO_SIGNUPEXCEPTION_H

#include "exception"

class SignupException: std::exception{
private:
    const char* cause;
public:
    SignupException(const char* cause){
        this->cause = cause;
    };
    const char* what() const noexcept(true){
        return this->cause;
    }
};

#endif //PROGETTO_SIGNUPEXCEPTION_H
