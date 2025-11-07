#pragma once

#include "../common/Socket.hpp"
#include "../common/Config.hpp"
#include "SessionManager.hpp"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <atomic>
#include <thread>

namespace pgw_server
{
    class UDPServer
    {
    public:
        static std::unique_ptr<UDPServer> UDP_server_init(
            const std::shared_ptr<spdlog::logger>& _logger,
            const pgw_common::ServerConfig& config_json,
            const std::shared_ptr<SessionManager>& sesManager);
        
        void receiveRequests();
        void sendResponse(const std::string& res, sockaddr_in& client_addr,
            socklen_t len);
        
        void setStopSign(bool new_stop_sign);

        void start();
        void stop();
    private:
        UDPServer(const std::shared_ptr<spdlog::logger>& _logger,
            std::unique_ptr<pgw_common::Socket> udp_socket,
            const std::shared_ptr<SessionManager>& sesManager);
        
        void run();

        std::shared_ptr<spdlog::logger> _svr_logger;
        std::unique_ptr<pgw_common::Socket> _udp_socket;
        std::shared_ptr<SessionManager> _sesManager;
        std::atomic<bool> _running{false};
        std::thread _udp_svr_thread;
    };
}