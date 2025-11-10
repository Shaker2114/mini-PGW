#include <gtest/gtest.h>
#include "UDPServer.hpp"
#include <spdlog/sinks/null_sink.h>

TEST(UDPServerTest, CreateUDPServer) {
    auto logger = std::make_shared<spdlog::logger>("test", 
        std::make_shared<spdlog::sinks::null_sink_mt>());
    
    pgw_common::ServerConfig config{};
    config.udp_ip = "0.0.0.0";
    config.udp_port = 9000;
    
    auto session_manager = std::make_shared<pgw_server::SessionManager>(
        30, "test_cdr.log", logger);
    
    EXPECT_NO_THROW({
        auto server = pgw_server::UDPServer::UDP_server_init(
            logger, config, session_manager);
        EXPECT_NE(server, nullptr);
    });
    
    std::filesystem::remove("log/test_cdr.log");
}