#ifndef CREATE_REVIEWEXCEPTION_H
#define CREATE_REVIEWEXCEPTION_H

#include "iostream"
#include "exception"

#pragma once

using namespace std;

class CreateReviewException : public exception {
private:
    const char* cause;
public:
    CreateReviewException(const char* cause) {
        this->cause = cause;
    };
    const char* what() const noexcept(true) {
        return this->cause;
    }

};
#endif //CREATE_REVIEWEXCEPTION_H