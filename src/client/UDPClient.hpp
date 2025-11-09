#pragma once

#include "../common/Config.hpp"
#include "../common/Socket.hpp"
#include "../common/BCDConverter.hpp"
#include "../common/Logger.hpp"

namespace pgw_client
{
    class UDPClient
    {
    public:
        UDPClient(const std::string& imsi);
        void configure();
        void serverInteraction();
    private:
        pgw_common::ClientConfig _client_config_json;
        std::unique_ptr<pgw_common::Socket> _udp_socket;
        std::shared_ptr<spdlog::logger> _client_logger;
        std::string _imsi;
    };
}