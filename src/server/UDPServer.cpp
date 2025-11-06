#include "UDPServer.hpp"
#include "../common/BCDConverter.hpp"

namespace pgw_server
{
    UDPServer::UDPServer(const std::shared_ptr<spdlog::logger>& _logger,
        std::unique_ptr<pgw_common::Socket> udp_socket,
        std::unique_ptr<SessionManager> sesManager)
        : _svr_logger(_logger), _sesManager(std::move(sesManager)),
        _udp_socket(std::move(udp_socket))
    {
        _svr_logger->info("UDP_server created with address: {}:{}",
            _udp_socket->getSocketAddr().sin_addr.s_addr,
            _udp_socket->getSocketAddr().sin_port);
    }

    std::unique_ptr<UDPServer> UDPServer::UDP_server_init(
            const std::shared_ptr<spdlog::logger>& _logger,
            const nlohmann::json& config_json)
    {
        try {
            _logger->info("Attempt to create UDP_server");

            std::string udp_ip = config_json["udp_ip"].get<std::string>();
            int udp_port = config_json["udp_port"].get<int>();
            _logger->info("UDP ip/port received");
            
            int timeout_sec = config_json["session_timeout_sec"].get<int>();
            std::string log_file = config_json["log_file"].get<std::string>();
            _logger->info("Session_timeout/log_file received");
            
            auto udp_socket = pgw_common::Socket::socket_init(udp_ip, udp_port);
            _logger->info("UDP socket created");

            auto sesManager = std::make_unique<SessionManager>(timeout_sec, log_file);
            _logger->info("SessionManager created");

            return std::make_unique<UDPServer>(_logger, std::move(udp_socket), std::move(sesManager));
        } catch(const std::exception& ex) {
            std::string error_message = "Failed to create UDP_server: " + std::string(ex.what());
            _logger->error(error_message);
            throw std::runtime_error(error_message);
        }
    }
    
    void UDPServer::setStopSign(bool new_stop_sign)
    {
        _stop_sign = new_stop_sign;
    }

    void UDPServer::receiveRequests()
    {
        while (_stop_sign)
        {
            char bcd_buffer[100]; // 100 byte
            sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);

            int number_of_bytes_read = recvfrom(_udp_socket->getSocketId(), (char *)bcd_buffer,
                99, MSG_WAITALL, (sockaddr*)(&client_addr), &len);

            if (number_of_bytes_read < 0) {
                _svr_logger->error("recvfrom() failed and RequestsHandler stopped");
                break;
            }

            bcd_buffer[number_of_bytes_read] = '\0';

            try {
                // uint8_t bcd_numbers[]
                // std::string bcd_data(bcd_buffer);
                // need to get std::vector<uint8_t> from char[]
            
                std::string imsi = pgw_common::BCDConverter::bcd_to_imsi(bcd_data);
                
                if (_sesManager->createSession(imsi)) {
                    _svr_logger->info("Session with imsi={} created", imsi);
                    sendResponse("created", client_addr, len);
                }
                else {
                    _svr_logger->info("Failed to create session with imsi={}", imsi);
                    sendResponse("rejected", client_addr, len);
                }
            } catch(const std::invalid_argument& ex) {
                std::string error_message = "Failed to convert BCD to imsi: " + std::string(ex.what());
                _svr_logger->error(error_message);
                throw std::invalid_argument(error_message);
            }
        }
    }
    
    void UDPServer::sendResponse(const std::string& res, sockaddr_in& client_addr, socklen_t len)
    {
        sendto(_udp_socket->getSocketId(), res.c_str(), res.length(),
            MSG_CONFIRM, (const sockaddr*)&client_addr, len);
        
        _svr_logger->info("Response sent: {}", res);
    }
}