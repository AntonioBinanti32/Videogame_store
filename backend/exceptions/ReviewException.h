#ifndef VIDEOGAME_STORE_REVIEWEXCEPTION_H
#define VIDEOGAME_STORE_REVIEWEXCEPTION_H

#include "iostream"
#include "exception"

class ReviewException : public std::exception {
private:
    const char* cause;
public:
    ReviewException(const char* cause) {
        this->cause = cause;
    };
    const char* what() const noexcept(true) {
        return this->cause;
    }

};

#endif //VIDEOGAME_STORE_REVIEWEXCEPTION_H