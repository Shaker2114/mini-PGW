#include "Config.hpp"

#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace pgw_common
{
    ServerConfig ServerConfig::load_config(const std::string& file_name)
    {
        std::ifstream cfgfile(file_name);
        
        if (!cfgfile.is_open()) {
            throw std::runtime_error("Failed to open config file: " + file_name);
        }

        try {
            nlohmann::json config_json = nlohmann::json::parse(cfgfile); // ex

            ServerConfig svr_cfg{};
            svr_cfg.udp_ip = config_json["udp_ip"].get<std::string>();
            svr_cfg.udp_port = config_json["udp_port"].get<int>();
            svr_cfg.session_timeout_sec = config_json["session_timeout_sec"].get<int>();
            svr_cfg.cdr_file = config_json["cdr_file"].get<std::string>();
            svr_cfg.http_port = config_json["http_port"].get<int>();
            svr_cfg.graceful_shutdown_rate = config_json["graceful_shutdown_rate"].get<int>();
            svr_cfg.log_file = config_json["log_file"].get<std::string>();
            svr_cfg.log_level = config_json["log_level"].get<std::string>();
            svr_cfg.blacklist = config_json["blacklist"].get<std::vector<std::string>>();

            return svr_cfg;
        } catch(const nlohmann::json::parse_error& ex) {
            throw std::runtime_error("JSON parse error in " + file_name + ": " + ex.what());
        }
    }

    ClientConfig ClientConfig::load_config(const std::string& file_name)
    {
        std::ifstream cfgfile(file_name);
        
        if (!cfgfile.is_open()) {
            throw std::runtime_error("Failed to open config file: " + file_name);
        }

        try {
            nlohmann::json config_json = nlohmann::json::parse(cfgfile); // ex

            ClientConfig client_cfg{};
            client_cfg.server_ip = config_json["server_ip"].get<std::string>();
            client_cfg.server_port = config_json["server_port"].get<int>();
            client_cfg.log_file = config_json["log_file"].get<std::string>();
            client_cfg.log_level = config_json["log_level"].get<std::string>();
            
            return client_cfg;
        } catch(const nlohmann::json::parse_error& ex) {
            throw std::runtime_error("JSON parse error in " + file_name + ": " + ex.what());
        }
    }
}