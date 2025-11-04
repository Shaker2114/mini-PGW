#include "Config.hpp"

#include <fstream>
#include <stdexcept>

namespace pgw_common
{
    nlohmann::json Config::get_config_json(const std::string& file_name)
    {
        std::ifstream cfgfile(file_name);
        
        if (!cfgfile.is_open()) {
            throw std::runtime_error("Failed to open config file: " + file_name);
        }

        try { 
            nlohmann::json config_json = nlohmann::json::parse(cfgfile);
            cfgfile.close();

            return config_json;
        } catch(const nlohmann::json::parse_error& ex) {
            throw std::runtime_error("JSON parse error in " + file_name + ": " + ex.what());
        }
    }
}