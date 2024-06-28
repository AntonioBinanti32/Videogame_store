#ifndef VIDEOGAME_STORE_CREATEGAMEEXCEPTION_H
#define VIDEOGAME_STORE_CREATEGAMEEXCEPTION_H

#include "iostream"
#include "exception"

#pragma once

using namespace std;

class CreateGameException : public exception {
private:
    const char* cause;
public:
    CreateGameException(const char* cause) {
        this->cause = cause;
    };
    const char* what() const noexcept(true) {
        return this->cause;
    }

};
#endif //VIDEOGAME_STORE_CREATEGAMEEXCEPTION_H