#include <gtest/gtest.h>
#include "Logger.hpp"
#include <filesystem>

TEST(LoggerTest, CreateLoggerWithInfoLevel) {
    EXPECT_NO_THROW({
        auto logger = pgw_common::Logger::logger_init("test1", "test_log1.log", "INFO");
        EXPECT_NE(logger, nullptr);
    });
}

TEST(LoggerTest, CreateLoggerWithDebugLevel) {
    EXPECT_NO_THROW({
        auto logger = pgw_common::Logger::logger_init("test2", "test_log2.log", "DEBUG");
        EXPECT_NE(logger, nullptr);
    });
}

TEST(LoggerTest, InvalidLogLevelDefaultsToInfo) {
    auto logger = pgw_common::Logger::logger_init("test3", "test_log3.log", "INVALID");
    EXPECT_NE(logger, nullptr);
    EXPECT_EQ(logger->level(), spdlog::level::info);
}

TEST(LoggerTest, CleanupTestFiles) {
    std::filesystem::remove("log/test_log1.log");
    std::filesystem::remove("log/test_log2.log");
    std::filesystem::remove("log/test_log3.log");
}