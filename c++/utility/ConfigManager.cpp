#include "ConfigManager.h"

namespace apl::utility{
    nlohmann::basic_json<> loadConfigFromFile(const char *configPath) {
        std::ifstream config_file(configPath);
        if (config_file.is_open()) {
            try {
                nlohmann::json json_data;
                config_file >> json_data;
                return json_data;
            } catch (const std::exception &e) {
                std::cerr << "Errore caricamento configuratione: " << e.what() << std::endl;
                exit(1);
            }
        } else {
            std::cerr << "Impossibile aprire il file di configurazione" << std::endl;
            exit(1);
        }
    }
}
