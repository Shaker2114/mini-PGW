#include "UDPServer.hpp"
#include "../common/BCDConverter.hpp"

#include <algorithm>

namespace pgw_server
{
    std::unique_ptr<UDPServer> UDPServer::UDP_server_init(
            const std::shared_ptr<spdlog::logger>& _logger,
            const pgw_common::ServerConfig& config_json,
            const std::shared_ptr<SessionManager>& sesManager)
    {
        try {
            _logger->info("Attempt to create UDP_server");
            
            auto udp_socket = pgw_common::Socket::socket_init(config_json.udp_ip, config_json.udp_port); // ex
            _logger->info("UDP socket created");

            return std::make_unique<UDPServer>(_logger, std::move(udp_socket), sesManager); // ex
        } catch(const std::exception& ex) {
            std::string error_message = "Failed to create UDP_server: " + std::string(ex.what());
            _logger->error(error_message);
            throw std::runtime_error(error_message);
        }
    }

    UDPServer::UDPServer(const std::shared_ptr<spdlog::logger>& _logger,
        std::unique_ptr<pgw_common::Socket> udp_socket,
        const std::shared_ptr<SessionManager>& sesManager)
        : _svr_logger(_logger), _sesManager(sesManager),
        _udp_socket(std::move(udp_socket))
    {
        _svr_logger->info("UDP_server created with address: {}:{}",
            _udp_socket->getSocketAddr().sin_addr.s_addr,
            _udp_socket->getSocketAddr().sin_port);
    }
    
    void UDPServer::setStopSign(bool new_stop_sign)
    {
        _running = new_stop_sign;
    }

    void UDPServer::receiveRequests()
    {
        while (_running)
        {
            unsigned char bcd_buffer[100]; // 100 byte
            sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);

            int number_of_bytes_read = recvfrom(_udp_socket->getSocketId(), (char *)bcd_buffer,
                99, MSG_WAITALL, reinterpret_cast<sockaddr*>(&client_addr), &len);

            if (number_of_bytes_read < 0) {
                _svr_logger->error("recvfrom() failed and RequestsHandler stopped");
                break;
            }

            bcd_buffer[number_of_bytes_read] = '\0';

            try {
                std::vector<uint8_t> bcd_data(number_of_bytes_read);

                for (size_t i = 0; i < number_of_bytes_read; ++i) {
                    bcd_data[i] = bcd_buffer[i] - '0';
                }

                std::string imsi = pgw_common::BCDConverter::bcd_to_imsi(bcd_data); // ex
                _svr_logger->info("client request received: {}", imsi);

                if (_sesManager->createSession(imsi)) {
                    _svr_logger->info("Session with imsi={} created", imsi);
                    sendResponse("created", client_addr, len);
                }
                else {
                    _svr_logger->info("Failed to create session with imsi={}", imsi);
                    sendResponse("rejected", client_addr, len);
                }

                _sesManager->checkExpiredSessions();

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
            MSG_CONFIRM, reinterpret_cast<const sockaddr*>(&client_addr), len);
        
        _svr_logger->info("Response sent: {}", res);
    }
}