#ifndef PROGETTO_HANDLER_H
#define PROGETTO_HANDLER_H

#include "../SocketTCP.h"
#include "../database/MongoDB.h"
#include "../exceptions/SocketException.h"
#include "../exceptions/LoginException.h"
#include "../minio-client/MinIOUploader.h"
#include "restbed"
#include <nlohmann/json.hpp>
#include "jwt/jwt.hpp"
#include <list>
#include "string.h"
#include "fstream"


namespace apl::handler {
    void signup_handler(const std::shared_ptr< restbed::Session > session);
    void login_handler(const std::shared_ptr< restbed::Session > session);
    void upload_image_handler(const std::shared_ptr< restbed::Session > session);
    void get_image_handler(const std::shared_ptr< restbed::Session > session);
    void like_handler(const std::shared_ptr< restbed::Session > session);
};


#endif //PROGETTO_HANDLER_H
