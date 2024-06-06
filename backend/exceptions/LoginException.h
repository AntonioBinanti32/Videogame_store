#ifndef VIDEOGAME_STORE_LOGINEXCEPTION_H
#define VIDEOGAME_STORE_LOGINEXCEPTION_H

#include "iostream"
#include "exception"

class LoginException : public std::exception {
private:
    const char* cause;
public:
    LoginException(const char* cause) {
        this->cause = cause;
    };
    const char* what() const noexcept(true) {
        return this->cause;
    }

};

#endif //VIDEOGAME_STORE_LOGINEXCEPTION_H