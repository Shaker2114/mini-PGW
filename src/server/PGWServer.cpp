#include "PGWServer.hpp"
#include "../common/Logger.hpp"
#include "../common/Config.hpp"

namespace pgw_server
{
    PGWServer PGWServer::PGW_server_init()
    {
        try {
            std::string filename_json = "pgw_server_cfg.json";
            pgw_common::ServerConfig svr_cfg = pgw_common::ServerConfig::load_config(filename_json); // ex

            auto _logger = pgw_common::Logger::logger_init("Server", svr_cfg.log_file, svr_cfg.log_level); // ex
            // можно добавить в sesaManager логгер
            auto sesManager = std::make_shared<SessionManager>(svr_cfg.session_timeout_sec, svr_cfg.cdr_file); // ex
            sesManager->setBlacklist(svr_cfg.blacklist); 
            _logger->info("SessionManager created");

            auto _udp_serv = UDPServer::UDP_server_init(_logger, svr_cfg, sesManager); // ex

            auto _http_serv = std::make_unique<HTTPServer>(_logger, svr_cfg); // ex

            return PGWServer(svr_cfg, _logger, sesManager, std::move(_udp_serv), std::move(_http_serv));
        } catch(const std::exception& ex) {
            std::string error_message = "Failed to create PGW server: " + std::string(ex.what());
            spdlog::critical(error_message);
        }
    }

    PGWServer::PGWServer(const pgw_common::ServerConfig& config_json,
        const std::shared_ptr<spdlog::logger>& _logger,
        const std::shared_ptr<SessionManager>& sesManager,
        std::unique_ptr<UDPServer> udp_serv,
        std::unique_ptr<HTTPServer> http_serv)
        : _svr_config_json(config_json), _svr_logger(_logger),
        _sesManager(sesManager), _udp_svr(std::move(udp_serv)),
        _http_svr(std::move(http_serv))
    {
        _svr_logger->info("The PGW server completely created");
    }

    void PGWServer::start()
    {
        _http_svr->start();
        _udp_svr->receiveRequests();
    }
}