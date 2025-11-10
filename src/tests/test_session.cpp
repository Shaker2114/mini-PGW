#include <gtest/gtest.h>
#include "Session.hpp"
#include <thread>

TEST(SessionTest, CreateSessionWithIMSI) {
    pgw_server::Session session("123456789012345");
    EXPECT_EQ(session.getIMSI(), "123456789012345");
}

TEST(SessionTest, SessionNotExpiredInitially) {
    pgw_server::Session session("123456789012345");
    EXPECT_FALSE(session.isExpired(30)); // 30 секунд таймаут
}

TEST(SessionTest, SessionExpiredAfterTimeout) {
    pgw_server::Session session("123456789012345");
    
    // Ждём немного (больше чем timeout)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    EXPECT_TRUE(session.isExpired(0));
}