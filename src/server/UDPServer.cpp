#include "UDPServer.hpp"
#include "../common/BCDConverter.hpp"

#include <fcntl.h>
#include <netinet/in.h>

namespace pgw_server
{
    std::unique_ptr<UDPServer> UDPServer::UDP_server_init(
            const std::shared_ptr<spdlog::logger>& _logger,
            const pgw_common::ServerConfig& config_json,
            const std::shared_ptr<SessionManager>& sesManager)
    {
        try {
            _logger->info("Attempt to create UDP_server");
            
            auto udp_socket = pgw_common::Socket::socket_init(
                "Server",
                config_json.udp_ip,
                config_json.udp_port
            ); // ex
            _logger->info("UDP socket created");

            return std::unique_ptr<UDPServer>(new UDPServer(
                _logger,
                std::move(udp_socket),
                sesManager,
                config_json
            )); //ex
        } catch(const std::exception& ex) {
            std::string error_message = "Failed to create UDP_server: " + std::string(ex.what());
            _logger->error(error_message);
            throw std::runtime_error(error_message);
        }
    }

    UDPServer::UDPServer(const std::shared_ptr<spdlog::logger>& _logger,
        std::unique_ptr<pgw_common::Socket> udp_socket,
        const std::shared_ptr<SessionManager>& sesManager,
        const pgw_common::ServerConfig& config_json)
        : _svr_logger(_logger), _sesManager(sesManager),
        _udp_socket(std::move(udp_socket))
    {
        _svr_logger->info("UDP Server created with address: {}:{}",
            config_json.udp_ip, config_json.udp_port);
    }

    void UDPServer::start()
    {
        _running = true;
        _udp_svr_thread = std::thread(&UDPServer::run, this);
    }

    void UDPServer::stop()
    {
        _running = false;
        if (_udp_svr_thread.joinable()) {
            _udp_svr_thread.join();
        }
    }

    void UDPServer::run()
    {
        _svr_logger->info("UDP server starting on port {}", ntohs(_udp_socket->getSocketAddr().sin_port));
        // non-blocking socket
        int flags = fcntl(_udp_socket->getSocketId(), F_GETFL, 0);
        fcntl(_udp_socket->getSocketId(), F_SETFL, flags | O_NONBLOCK);

        while (_running)
        {
            unsigned char bcd_buffer[100]; // 100 byte
            sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);

            int number_of_bytes_read = recvfrom(
                _udp_socket->getSocketId(),
                reinterpret_cast<char*>(bcd_buffer),
                sizeof(bcd_buffer) - 1,
                0,
                reinterpret_cast<sockaddr*>(&client_addr),
                &len
            );

            if (number_of_bytes_read < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // no data
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                }
                else {
                    _svr_logger->error("recvfrom() error: {}", strerror(errno));
                    break;
                }
            }

            try {
                std::vector<uint8_t> bcd_data(bcd_buffer, bcd_buffer + number_of_bytes_read);

                std::string imsi = pgw_common::BCDConverter::bcd_to_imsi(bcd_data); // ex
                _svr_logger->info("client request received: {}", imsi);

                std::string response = (_sesManager->createSession(imsi)) ? "created" : "rejected";

                ssize_t sent = sendto(
                    _udp_socket->getSocketId(),
                    response.c_str(),
                    response.length(),
                    0,
                    reinterpret_cast<const sockaddr*>(&client_addr),
                    len
                );

                if (sent < 0) {
                    _svr_logger->warn("Failed to send response to client: {}", strerror(errno));
                } else {
                    _svr_logger->info(
                        "Response: '{}' sent to client with imsi={}",
                        response,
                        imsi
                    );
                }

            } catch(const std::invalid_argument& ex) {
                _svr_logger->error("Packet processing failed: {}", ex.what());
                continue;
            }
        }

        _svr_logger->info("The UDP server has stopped");
    }
}