#include "HTTPServer.hpp"

namespace pgw_server
{
    HTTPServer::HTTPServer(const std::shared_ptr<spdlog::logger>& _logger,
        const std::shared_ptr<SessionManager>& sesManager,
        int port)
        : _svr_logger(_logger), _sesManager(sesManager), _port(port)
    {
        _svr_logger->info("HTTP Server created. Starting to set it up.");
        
        _http_svr.Get("/check_subscriber", [this](const httplib::Request& req, httplib::Response& res){
            std::string imsi = req.get_param_value("imsi");

            std::string response_content = (_sesManager->hasSession(imsi)) ? "active" : "not active";
            
            res.set_content(response_content, "text/plain");
        });

        _svr_logger->info("GET /check_subscriber is configured");

        _http_svr.Get("/stop", [this](const httplib::Request& req, httplib::Response& res) {
            _svr_logger->info("HTTPServer's starting shutdown");
            res.set_content("Starting graceful shutdown...", "text/plain");
            while (_sesManager->getSessionsQuantity() != 0) {
                _sesManager->shutdownSessions(5); // need to graceful_sd_rate from cfg_json
            }
            // !!!
        });
        
        _svr_logger->info("GET /stop is configured");
        _svr_logger->info("HTTP Server is configured");
    }
    void HTTPServer::start()
    {
        _http_svr_thread = std::thread(&HTTPServer::run, this);
    }
    void HTTPServer::stop()
    {
        if (_http_svr_thread.joinable()) {
            _http_svr_thread.join();
        }
        _http_svr.stop();
        _svr_logger->info("The HTTP Server has stopped");
    }
    void HTTPServer::run()
    {
        _http_svr.listen("0.0.0.0", _port);
    }
}