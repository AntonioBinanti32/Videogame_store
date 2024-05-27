#ifndef VIDEOGAME_STORE_HANDLER_H
#define VIDEOGAME_STORE_HANDLER_H

//TODO: fare socket
//#include "../socket/SocketTCP.h"
#include "../database/MongoDB.h"
//TODO: Fare exceptions
//#include "../exceptions/SocketException.h"
//#include "../exceptions/LoginException.h"
//#include "../exceptions/SignupException.h"
//#include "../exceptions/UploadException.h"
#include "restbed"
#include <nlohmann/json.hpp>
#include "jwt/jwt.hpp"
#include <list>
#include "string.h"
#include "fstream"

#pragma once

namespace videogame_store::handler {
    void signup_handler(const std::shared_ptr< restbed::Session > session);
    void login_handler(const std::shared_ptr< restbed::Session > session);
    void upload_handler(const std::shared_ptr< restbed::Session > session);
    //TODO: Implementare altre dichiarazioni
};


#endif //VIDEOGAME_STORE_HANDLER_H