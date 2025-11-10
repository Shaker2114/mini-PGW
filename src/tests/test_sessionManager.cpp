#include <gtest/gtest.h>
#include "SessionManager.hpp"
#include <spdlog/sinks/null_sink.h>
#include <filesystem>

std::shared_ptr<spdlog::logger> createTestLogger() {
    auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
    return std::make_shared<spdlog::logger>("test", null_sink);
}

TEST(SessionManagerTest, CreateSessionManager) {
    auto logger = createTestLogger();
    EXPECT_NO_THROW({
        pgw_server::SessionManager manager(30, "test_sm1.log", logger);
    });
}

TEST(SessionManagerTest, CreateAndRemoveSession) {
    auto logger = createTestLogger();
    pgw_server::SessionManager manager(30, "test_sm2.log", logger);
    
    EXPECT_TRUE(manager.createSession("123456789012345"));
    EXPECT_TRUE(manager.hasSession("123456789012345"));
    EXPECT_TRUE(manager.removeSession("123456789012345"));
    EXPECT_FALSE(manager.hasSession("123456789012345"));
}

TEST(SessionManagerTest, BlacklistRejection) {
    auto logger = createTestLogger();
    pgw_server::SessionManager manager(30, "test_sm3.log", logger);
    
    manager.setBlacklist({"111111111111111"});
    EXPECT_FALSE(manager.createSession("111111111111111"));
    EXPECT_TRUE(manager.createSession("222222222222222"));
}

TEST(SessionManagerTest, SessionQuantity) {
    auto logger = createTestLogger();
    pgw_server::SessionManager manager(30, "test_sm4.log", logger);
    
    EXPECT_EQ(manager.getSessionsQuantity(), 0);
    manager.createSession("123456789012345");
    EXPECT_EQ(manager.getSessionsQuantity(), 1);
}

TEST(SessionManagerTest, CleanupTestFiles) {
    std::filesystem::remove("log/test_sm1.log");
    std::filesystem::remove("log/test_sm2.log");
    std::filesystem::remove("log/test_sm3.log");
    std::filesystem::remove("log/test_sm4.log");
}