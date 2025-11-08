#include "HTTPServer.hpp"

namespace pgw_server
{
    HTTPServer::HTTPServer(const std::shared_ptr<spdlog::logger>& _logger, 
        int port,
        checkSubscriberHandler check_handler,
        stopHandler stop_handler)
        : _svr_logger(_logger), _port(port),
        _check_handler(std::move(check_handler)),
        _stop_handler(std::move(stop_handler))
    {   
        _http_svr.Get("/check_subscriber", [this](const httplib::Request& req, httplib::Response& res){
            std::string imsi = req.get_param_value("imsi");

            if (imsi.empty()) {
                res.set_content("Error: imsi parameter required", "text/plain");
                res.status = 400;
                return;
            }

            std::string response_content = (_check_handler(imsi)) ? "active" : "not active";
            
            res.set_content(response_content + "\n", "text/plain");
        });

        _http_svr.Get("/stop", [this](const httplib::Request& req, httplib::Response& res) {
            _svr_logger->info("Received shutdown command with HTTP");
            res.set_content("Starting graceful shutdown...\n", "text/plain");
            
            std::thread([this]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                _stop_handler();
            }).detach();
        });
        
        _svr_logger->info("HTTP Server with address 0.0.0.0:{} created and configured", _port);
    }

    void HTTPServer::start()
    {
        _http_svr_thread = std::thread(&HTTPServer::run, this);
    }
    void HTTPServer::stop()
    {
        _http_svr.stop();

        if (_http_svr_thread.joinable()) {
            _http_svr_thread.join();
        }

        _svr_logger->info("The HTTP server has stopped");
    }
    void HTTPServer::run()
    {
        try {
            _svr_logger->info("HTTP Server starting on port {}", _port);
            if (!_http_svr.listen("0.0.0.0", _port)) {
                _svr_logger->error("HTTP Server failed to start on port {}", _port);
            }
        } catch(const std::exception& ex) {
            _svr_logger->critical("HTTP Server crashed: {}", ex.what());
        }

        _svr_logger->info("HTTP server thread has ended");
    }
}