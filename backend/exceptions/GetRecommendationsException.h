#ifndef GET_RECOMMENDATIONEXCEPTION_H
#define GET_RECOMMENDATIONEXCEPTION_H

#include "iostream"
#include "exception"

#pragma once

using namespace std;

class GetRecommendationsException : public exception {
private:
    const char* cause;
public:
    GetRecommendationsException(const char* cause) {
        this->cause = cause;
    };
    const char* what() const noexcept(true) {
        return this->cause;
    }

};
#endif //GET_RECOMMENDATIONEXCEPTION_H