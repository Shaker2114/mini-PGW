#include <gtest/gtest.h>
#include "PGWServer.hpp"

TEST(PGWServerTest, CreatePGWServer) {
    EXPECT_NO_THROW({
        pgw_server::PGWServer server;
    });
}