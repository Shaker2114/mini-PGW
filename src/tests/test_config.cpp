#include <gtest/gtest.h>
#include "Config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_dir = "test_configs";
        std::filesystem::create_directory(test_dir);
    }

    void TearDown() override {
        std::filesystem::remove_all(test_dir);
    }

    std::string test_dir;
};

TEST_F(ConfigTest, LoadValidServerConfig) {
    // Валидный конфиг сервера
    std::string config_file = test_dir + "/server_config.json";
    std::ofstream file(config_file);
    file << R"({
        "udp_ip": "0.0.0.0",
        "udp_port": 9000,
        "session_timeout_sec": 40,
        "cdr_file": "test_cdr.log",
        "http_port": 8080,
        "graceful_shutdown_rate": 20,
        "log_file": "test_pgw.log",
        "log_level": "DEBUG",
        "blacklist": ["001010123456789", "001010000000001", "012312310123"]
    })";
    file.close();

    auto config = pgw_common::ServerConfig::load_config(config_file);
    
    EXPECT_EQ(config.udp_ip, "0.0.0.0");
    EXPECT_EQ(config.udp_port, 9000);
    EXPECT_EQ(config.session_timeout_sec, 40);
    EXPECT_EQ(config.cdr_file, "test_cdr.log");
    EXPECT_EQ(config.http_port, 8080);
    EXPECT_EQ(config.graceful_shutdown_rate, 20);
    EXPECT_EQ(config.log_file, "test_pgw.log");
    EXPECT_EQ(config.log_level, "DEBUG");
    EXPECT_EQ(config.blacklist.size(), 3);
    EXPECT_EQ(config.blacklist[0], "001010123456789");
    EXPECT_EQ(config.blacklist[1], "001010000000001");
    EXPECT_EQ(config.blacklist[2], "012312310123");
}

TEST_F(ConfigTest, LoadValidClientConfig) {
    // Валидный конфиг клиента
    std::string config_file = test_dir + "/client_config.json";
    std::ofstream file(config_file);
    file << R"({
        "server_ip": "127.0.0.1",
        "server_port": 9000,
        "log_file": "test_client.log",
        "log_level": "DEBUG"
    })";
    file.close();

    auto config = pgw_common::ClientConfig::load_config(config_file);
    
    EXPECT_EQ(config.server_ip, "127.0.0.1");
    EXPECT_EQ(config.server_port, 9000);
    EXPECT_EQ(config.log_file, "test_client.log");
    EXPECT_EQ(config.log_level, "DEBUG");
}

TEST_F(ConfigTest, MissingConfigFile) {
    EXPECT_THROW(
        pgw_common::ServerConfig::load_config("nonexistent.json"),
        std::runtime_error
    );
}

TEST_F(ConfigTest, InvalidJSONFormat) {
    // Битый JSON
    std::string config_file = test_dir + "/broken_config.json";
    std::ofstream file(config_file);
    file << R"({
        "udp_ip": "0.0.0.0",
        "udp_port": 9000,
        "session_timeout_sec": 30,
        "cdr_file": "cdr.log",
        MISSING_COMMA
        "http_port": 8080
    })";
    file.close();

    EXPECT_THROW(
        pgw_common::ServerConfig::load_config(config_file),
        std::runtime_error
    );
}

TEST_F(ConfigTest, ConfigWithExtraFields) {
    // Конфиг с дополнительными полями
    std::string config_file = test_dir + "/extra_fields.json";
    std::ofstream file(config_file);
    file << R"({
        "udp_ip": "0.0.0.0",
        "udp_port": 9000,
        "session_timeout_sec": 30,
        "cdr_file": "cdr.log",
        "http_port": 8080,
        "graceful_shutdown_rate": 10,
        "log_file": "pgw.log",
        "log_level": "INFO",
        "blacklist": ["001010123456789"],
        "extra_field_1": "should_be_ignored",
        "unknown_field": 12345
    })";
    file.close();

    EXPECT_NO_THROW(
        auto config = pgw_common::ServerConfig::load_config(config_file);
        
        EXPECT_EQ(config.udp_ip, "0.0.0.0");
        EXPECT_EQ(config.udp_port, 9000);
        EXPECT_EQ(config.session_timeout_sec, 30);
        EXPECT_EQ(config.cdr_file, "cdr.log");
        EXPECT_EQ(config.http_port, 8080);
        EXPECT_EQ(config.graceful_shutdown_rate, 10);
        EXPECT_EQ(config.log_file, "pgw.log");
        EXPECT_EQ(config.log_level, "INFO");
        EXPECT_EQ(config.blacklist.size(), 1);
    );
}