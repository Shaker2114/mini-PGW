#pragma once

#include "UDPServer.hpp"
#include "HTTPServer.hpp"

namespace pgw_server
{
    class PGWServer
    {
    public:
        static PGWServer PGW_server_init();
        
        void run();
        void stop();

        

    private:
        PGWServer(const pgw_common::ServerConfig& config_json,
            const std::shared_ptr<spdlog::logger>& _logger,
            const std::shared_ptr<SessionManager>& sesManager,
            std::unique_ptr<UDPServer> udp_serv,
            std::unique_ptr<HTTPServer> http_serv);
        
        pgw_common::ServerConfig _svr_config_json;
        std::shared_ptr<spdlog::logger> _svr_logger;
        std::shared_ptr<SessionManager> _sesManager;
        std::unique_ptr<UDPServer> _udp_svr;
        std::unique_ptr<HTTPServer> _http_svr;
        std::atomic<bool> _running{true};
    };
}