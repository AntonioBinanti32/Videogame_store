#ifndef CREATE_RESERVATIONEXCEPTION_H
#define CREATE_RESERVATIONEXCEPTION_H

#include "iostream"
#include "exception"

#pragma once

using namespace std;

class CreateReservationException : public exception {
private:
    const char* cause;
public:
    CreateReservationException(const char* cause) {
        this->cause = cause;
    };
    const char* what() const noexcept(true) {
        return this->cause;
    }

};
#endif //CREATE_RESERVATIONEXCEPTION_H