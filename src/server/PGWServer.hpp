#pragma once

#include "UDPServer.hpp"
#include "HTTPServer.hpp"

namespace pgw_server
{
    class PGWServer
    {
    public:
        void run();
        void gracefulShutdown();
    
    private:
        void configure();
        
        pgw_common::ServerConfig _svr_config_json;
        std::shared_ptr<spdlog::logger> _svr_logger;
        std::shared_ptr<SessionManager> _sesManager;
        std::unique_ptr<UDPServer> _udp_svr;
        std::unique_ptr<HTTPServer> _http_svr;
        std::atomic_bool _running{true};
    };
}