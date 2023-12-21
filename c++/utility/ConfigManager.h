#ifndef PROGETTO_CONFIGMANAGER_H
#define PROGETTO_CONFIGMANAGER_H

#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>

namespace apl::utility {
    nlohmann::basic_json<> loadConfigFromFile(const char *configPath);
}


#endif //PROGETTO_CONFIGMANAGER_H
