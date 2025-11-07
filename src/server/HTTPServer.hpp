#pragma once

#include "../common/Config.hpp"
#include "SessionManager.hpp"

#include <httplib.h>
#include <spdlog/spdlog.h>
#include <thread>
// #include <functional>

namespace pgw_server
{
    class HTTPServer
    {
    public:
        HTTPServer(const std::shared_ptr<spdlog::logger>& _logger, 
            const std::shared_ptr<SessionManager>& sesManager,
            int port);
        
        void start();
        void stop();

    private:
        void run();

        httplib::Server _http_svr;
        std::shared_ptr<spdlog::logger> _svr_logger;
        std::shared_ptr<SessionManager> _sesManager;
        int _port;
        std::thread _http_svr_thread;
    };
}