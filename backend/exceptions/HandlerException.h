#ifndef VIDEOGAME_STORE_HANDLEREXCEPTION_H
#define VIDEOGAME_STORE_HANDLEREXCEPTION_H

#include "iostream"
#include "exception"

#pragma once

using namespace std;

class HandlerException : public exception {
private:
    const char* cause;
public:
    HandlerException(const char* cause) {
        this->cause = cause;
    };
    const char* what() const noexcept(true) {
        return this->cause;
    }

};
#endif //VIDEOGAME_STORE_HANDLEREXCEPTION_H