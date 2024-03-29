#ifndef PROGETTO_HANDLER_H
#define PROGETTO_HANDLER_H

#include "../socket/SocketTCP.h"
#include "../database/MongoDB.h"
#include "../exceptions/SocketException.h"
#include "../exceptions/LoginException.h"
#include "../exceptions/SignupException.h"
#include "../exceptions/UploadException.h"
#include "../minio-client/MinIOUploader.h"
#include "restbed"
#include <nlohmann/json.hpp>
#include "jwt/jwt.hpp"
#include <list>
#include "string.h"
#include "fstream"

#pragma once

namespace apl::handler {
    void signup_handler(const std::shared_ptr< restbed::Session > session);
    void login_handler(const std::shared_ptr< restbed::Session > session);
    void upload_image_handler(const std::shared_ptr< restbed::Session > session);
    void get_image_handler(const std::shared_ptr< restbed::Session > session);
    void like_handler(const std::shared_ptr< restbed::Session > session);
};


#endif //PROGETTO_HANDLER_H
