#pragma once

#include "../common/Config.hpp"
#include "SessionManager.hpp"

#include <httplib.h>
#include <spdlog/spdlog.h>
#include <thread>
#include <functional>

namespace pgw_server
{
    class HTTPServer
    {
    private:
        using checkSubscriberHandler = std::function<bool(const std::string&)>;
        using stopHandler = std::function<void()>;
    public:
        HTTPServer(const std::shared_ptr<spdlog::logger>& _logger, 
            int port,
            checkSubscriberHandler check_handler,
            stopHandler stop_handler);
        
        void start();
        void stop();

    private:
        void run();

        httplib::Server _http_svr;
        std::shared_ptr<spdlog::logger> _svr_logger;
        int _port;
        std::thread _http_svr_thread;

        checkSubscriberHandler _check_handler;
        stopHandler _stop_handler;
    };
}