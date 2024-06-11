#ifndef VIDEOGAME_STORE_RESERVATIONEXCEPTION_H
#define VIDEOGAME_STORE_RESERVATIONEXCEPTION_H

#include "iostream"
#include "exception"

class ReservationException : public std::exception {
private:
    const char* cause;
public:
    ReservationException(const char* cause) {
        this->cause = cause;
    };
    const char* what() const noexcept(true) {
        return this->cause;
    }

};

#endif //VIDEOGAME_STORE_RESERVATIONEXCEPTION_H