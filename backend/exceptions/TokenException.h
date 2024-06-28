#ifndef VIDEOGAME_STORE_TOKENEXCEPTION_H
#define VIDEOGAME_STORE_TOKENEXCEPTION_H

#include "iostream"
#include "exception"

#pragma once

using namespace std;

class TokenException : public exception {
private:
    const char* cause;
public:
    TokenException(const char* cause) {
        this->cause = cause;
    };
    const char* what() const noexcept(true) {
        return this->cause;
    }

};
#endif //VIDEOGAME_STORE_TOKENEXCEPTION_H