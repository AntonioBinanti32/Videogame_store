#ifndef PURCHASEEXCEPTION_H
#define PURCHASEEXCEPTION_H

#include "iostream"
#include "exception"

#pragma once

using namespace std;

class PurchaseException : public exception {
private:
    const char* cause;
public:
    PurchaseException(const char* cause) {
        this->cause = cause;
    };
    const char* what() const noexcept(true) {
        return this->cause;
    }

};
#endif //PURCHASEEXCEPTION_H