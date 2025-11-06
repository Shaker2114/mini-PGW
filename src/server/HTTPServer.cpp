#include "HTTPServer.hpp"

namespace pgw_server
{
    HTTPServer::HTTPServer(spdlog::logger* Logger)
        : _logger(Logger)
    {
        _logger->debug("HTTP Server created. Starting to set it up.");
        
        _svr.Get("/check_subscriber", [](const httplib::Request& req, httplib::Response& res){
            std::string imsi = req.get_param_value("imsi");
            // проверить существует ли сессия с данным imsi:
            // если да - вернуть "active", else - "not active";
            std::string response_content = "Полученный imsi: " + imsi + "\n";
            res.set_content(response_content, "text/plain");
        });

        _logger->debug("GET /check_subscriber is configured");

        _svr.Get("/stop", [this](const httplib::Request& req, httplib::Response& res) {
            // _logger->info("Server's starting shutdown");
            res.set_content("Starting graceful shutdown...", "text/plain");
            // start shutdown...
        });
        
        _logger->debug("GET /stop is configured");
        _logger->info("HTTP Server is configured");
        
        // 8080 - http port from config (need to parse it from there)
        _svr.listen("0.0.0.0", 8080);
    }
    HTTPServer::~HTTPServer()
    {
        _svr.stop();
        _logger->debug("The HTTP Server has stopped");
    }
}