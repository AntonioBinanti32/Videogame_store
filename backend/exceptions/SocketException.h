#ifndef VIDEOGAME_STORE_SOCKETEXCEPTION_H
#define VIDEOGAME_STORE_SOCKETEXCEPTION_H

#include "exception"

class SocketException : std::exception {
private:
    const char* cause;
public:
    SocketException(const char* cause) {
        this->cause = cause;
    };
    const char* what() const noexcept(true) {
        return this->cause;
    }
};

#endif //VIDEOGAME_STORE_SOCKETEXCEPTION_H