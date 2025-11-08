#include "PGWServer.hpp"
#include "../common/Logger.hpp"
#include "../common/Config.hpp"

namespace pgw_server
{
    void PGWServer::configure()
    {
        if (_svr_logger) {
            throw std::runtime_error("PGWServer already configured");
        }

        try {
            // config
            std::string filename_json = "configs/pgw_server_cfg.json";
            _svr_config_json = pgw_common::ServerConfig::load_config(filename_json); // ex

            // logger
            _svr_logger = pgw_common::Logger::logger_init(
                "Server",
                _svr_config_json.log_file,
                _svr_config_json.log_level
            ); // ex

            // sessionManager
            _sesManager = std::make_shared<SessionManager>(
                _svr_config_json.session_timeout_sec,
                _svr_config_json.cdr_file
            ); // ex
            _sesManager->setBlacklist(_svr_config_json.blacklist); 
            _svr_logger->info("SessionManager created");

            // UDP server
            _udp_svr = UDPServer::UDP_server_init(
                _svr_logger,
                _svr_config_json,
                _sesManager
            ); // ex

            // HTTP server
            _http_svr = std::make_unique<HTTPServer>(
                _svr_logger,
                _svr_config_json.http_port,
                [this](const std::string& imsi) -> bool {
                    return _sesManager->hasSession(imsi);
                },
                [this]() -> void {
                    gracefulShutdown();
                }
            ); // ex
            
            _svr_logger->info("PGW Server configuration completed");

        } catch(const std::exception& ex) {
            std::string error_message = "Failed to configure PGW server: " + std::string(ex.what());
            if (_svr_logger) {
                _svr_logger->critical(error_message);
            }
            else {
                spdlog::critical(error_message);
            }
            
            throw std::runtime_error(error_message);
        }
    }

    void PGWServer::run()
    {
        // configure if it necessary
        if (!_svr_logger) {
            configure();
        }

        if (!_running) {
            _svr_logger->warn("PGW Server already stopped");
            return;
        }

        _svr_logger->info("Starting PGW Server...");

        _udp_svr->start();
        _http_svr->start();

        while (_running) {
            _sesManager->checkExpiredSessions();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        // waiting to gracefulShutdown in other thread end
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        _svr_logger->info("The PGW Server has stopped");
    }
    
    void PGWServer::gracefulShutdown()
    {
        if (!_running) return;

        _svr_logger->info("Starting graceful shutdown...");

        try {
            if (_http_svr) {
                _svr_logger->info("Stopping HTTP server...");
                _http_svr->stop();
            }

            if (_udp_svr) {
                _svr_logger->info("Stopping UDP server...");
                _udp_svr->stop();
            }

            if (_sesManager) {
                _svr_logger->info("Shutting down remaining sessions...");
                while (_sesManager->getSessionsQuantity() > 0) {
                    _svr_logger->info("Remaining sessions: {}", _sesManager->getSessionsQuantity());
                    _sesManager->shutdownSessions(_svr_config_json.graceful_shutdown_rate);
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
        } catch(const std::exception& ex) {
            _svr_logger->error("Error during shutdown: {}", ex.what());
        }

        _running = false;
        _svr_logger->info("Graceful shutdown completed");
    }
}