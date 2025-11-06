#pragma once

#include <httplib.h>
#include <spdlog/spdlog.h>

namespace pgw_server
{
    class HTTPServer
    {
    public:
        explicit HTTPServer(const std::shared_ptr<spdlog::logger>& _logger);
        ~HTTPServer();
    private:
        httplib::Server _svr;
        std::shared_ptr<spdlog::logger> _svr_logger;
    };
}