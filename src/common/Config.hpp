#pragma once

#include <vector>
#include <string>

namespace pgw_common
{
    struct ServerConfig
    {
        std::string udp_ip;
        int udp_port;
        int session_timeout_sec;
        std::string cdr_file;
        int http_port;
        int graceful_shutdown_rate;
        std::string log_file;
        std::string log_level;
        std::vector<std::string> blacklist;

        static ServerConfig load_config(const std::string& file_name);
    };

    struct ClientConfig
    {
        std::string server_ip;
        int server_port;
        std::string log_file;
        std::string log_level;

        static ClientConfig load_config(const std::string& file_name);
    };
}