#include "UDPClient.hpp"
#include <fcntl.h>

namespace pgw_client
{
    UDPClient::UDPClient(const std::string& imsi)
        : _imsi(imsi) {}
    
    void UDPClient::configure()
    {
        try {
            std::string cfg_filename = "configs/pgw_client_cfg.json";
            _client_config_json = pgw_common::ClientConfig::load_config(cfg_filename); // ex

            _client_logger = pgw_common::Logger::logger_init(
                "Client",
                _client_config_json.log_file,
                _client_config_json.log_level
            ); // ex

            _udp_socket = pgw_common::Socket::socket_init(
                "Client",
                _client_config_json.server_ip,
                _client_config_json.server_port
            ); // ex
            _client_logger->info(
                "UDP socket initialized with address {}:{}",
                _client_config_json.server_ip,
                _client_config_json.server_port
            );

            _client_logger->info("Client configuration with imsi={} completed", _imsi);

        } catch(const std::exception& ex) {
            std::string error_message = "Failed to configure client: " + std::string(ex.what());
            if (_client_logger) {
                _client_logger->critical(error_message);
            }
            else {
                spdlog::critical(error_message);
            }
            
            throw std::runtime_error(error_message);
        }
    }

    void UDPClient::serverInteraction()
    {
        try {
            _client_logger->info("Attempt to send UDP request for IMSI: {}", _imsi);
            // non-blocking socket
            int flags = fcntl(_udp_socket->getSocketId(), F_GETFL, 0);
            fcntl(_udp_socket->getSocketId(), F_SETFL, flags | O_NONBLOCK);
            // serv addr
            const sockaddr_in& server_addr = _udp_socket->getSocketAddr();
            socklen_t len = sizeof(server_addr);
            // imsi to bcd
            _client_logger->info("Converting IMSI to BCD...");
            std::vector<uint8_t> imsi_in_bcd = pgw_common::BCDConverter::imsi_to_bcd(_imsi); // ex
            // attempts to send request
            const int MAX_ATTEMPTS = 3;
            bool send_result = false;
            // try to send
            for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt)
            {
                ssize_t sent = sendto(
                    _udp_socket->getSocketId(),
                    imsi_in_bcd.data(),
                    imsi_in_bcd.size(),
                    0,
                    reinterpret_cast<const sockaddr*>(&server_addr),
                    len
                );

                if (sent < 0) {
                    _client_logger->warn(
                        "Send attempt {} failed: {}",
                        attempt + 1,
                        strerror(errno)
                    );
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                } else {
                    _client_logger->info(
                        "IMSI '{}' ({} bytes BCD) sent successfully", 
                        _imsi,
                        imsi_in_bcd.size()
                    );
                    send_result = true;
                    break;
                }
            }
            // failed to send
            if (!send_result) {
                throw std::runtime_error("Failed to send request after " + 
                    std::to_string(MAX_ATTEMPTS) + " attempts");
            }

            _client_logger->info("Waiting for server response...");
            // copy svr address for recvfrom
            unsigned char server_response[100]; // 100 byte
            sockaddr_in copy_serv_addr = server_addr;
            socklen_t cpy_len = sizeof(copy_serv_addr);
            bool got_response = false;
            const int MAX_SECONDS_TO_WAIT = 5;
            // wait response for 5 seconds then 
            auto start = std::chrono::steady_clock::now();
            while ((std::chrono::steady_clock::now() - start) < std::chrono::seconds(MAX_SECONDS_TO_WAIT))
            {
                int number_of_bytes_read = recvfrom(
                    _udp_socket->getSocketId(),
                    reinterpret_cast<char*>(server_response),
                    sizeof(server_response) - 1,
                    0,
                    reinterpret_cast<sockaddr*>(&copy_serv_addr),
                    &cpy_len
                );

                if (number_of_bytes_read < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        // no data
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        continue;
                    }
                    else {
                        throw std::runtime_error("recvfrom() error: " + std::string(strerror(errno)));
                    }
                }
                else {
                    got_response = true;
                    server_response[number_of_bytes_read] = '\0';
                    break;
                }
            }
            
            if (!got_response) {
                throw std::runtime_error("Failed to receive response after " + 
                    std::to_string(MAX_SECONDS_TO_WAIT) + " seconds");
            }
            // std::string response
            std::string svr_response = reinterpret_cast<char*>(server_response);

            if (svr_response != "created" && svr_response != "rejected") {
                _client_logger->warn("Unexpected server response: '{}'", svr_response);
            }
            else {
                _client_logger->info("Received server response: {}", svr_response);
            }
            // end of interaction with the server
            _client_logger->info("The Client has finished interaction with the server");
        
        } catch(const std::exception& ex) {
            std::string error_message = "Client error: " + std::string(ex.what());
            _client_logger->critical(error_message);
            throw std::runtime_error(error_message);
        }
    }
}