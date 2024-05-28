#ifndef VIDEOGAME_STORE_USERNOTFOUNDEXCEPTION_H
#define VIDEOGAME_STORE_USERNOTFOUNDEXCEPTION_H

#include "iostream"
#include "exception"

#pragma once

using namespace std;

class UserNotFoundException : public exception {
private:
    const char* cause;
public:
    UserNotFoundException(const char* cause) {
        this->cause = cause;
    };
    const char* what() const noexcept(true) {
        return this->cause;
    }

};
#endif //VIDEOGAME_STORE_USERNOTFOUNDEXCEPTION_H