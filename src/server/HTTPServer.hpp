#pragma once

#include <httplib.h>
#include <spdlog/spdlog.h>

namespace pgw_serv
{
    class HTTPServer
    {
    public:
        explicit HTTPServer(spdlog::logger* Logger);
        ~HTTPServer();
    private:
        httplib::Server _svr;
        spdlog::logger* _logger;
    };
}