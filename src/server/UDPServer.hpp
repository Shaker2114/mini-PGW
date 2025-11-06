#pragma once

#include "../common/Socket.hpp"
#include "SessionManager.hpp"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace pgw_server
{
    class UDPServer
    {
    public:
        static std::unique_ptr<UDPServer> UDP_server_init(
            const std::shared_ptr<spdlog::logger>& _logger,
            const nlohmann::json& config_json);
        
        void receiveRequests();
        void sendResponse(const std::string& res, sockaddr_in& client_addr,
            socklen_t len);
        
        void setStopSign(bool new_stop_sign);
    private:
        UDPServer(const std::shared_ptr<spdlog::logger>& _logger,
            std::unique_ptr<pgw_common::Socket> udp_socket,
            std::unique_ptr<SessionManager> sesManager);
        
        std::unique_ptr<pgw_common::Socket> _udp_socket;
        std::shared_ptr<spdlog::logger> _svr_logger;
        std::unique_ptr<SessionManager> _sesManager;
        bool _stop_sign = true;
    };
}