#ifndef VIDEOGAME_STORE_GETGAMEEXCEPTION_H
#define VIDEOGAME_STORE_GETGAMEEXCEPTION_H

#include "exception"

class GetGameException : std::exception {
private:
    const char* cause;
public:
    GetGameException(const char* cause) {
        this->cause = cause;
    };
    const char* what() const noexcept(true) {
        return this->cause;
    }
};

#endif //VIDEOGAME_STORE_GETGAMEEXCEPTION_H