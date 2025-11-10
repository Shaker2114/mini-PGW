#include <gtest/gtest.h>
#include "HTTPServer.hpp"
#include <spdlog/sinks/null_sink.h>

TEST(HTTPServerTest, CreateHTTPServer) {
    auto logger = std::make_shared<spdlog::logger>("test", 
        std::make_shared<spdlog::sinks::null_sink_mt>());
    
    auto check_handler = [](const std::string& imsi) { return true; };
    auto stop_handler = []() {};
    
    EXPECT_NO_THROW({
        pgw_server::HTTPServer server(logger, 8080, check_handler, stop_handler);
    });
}