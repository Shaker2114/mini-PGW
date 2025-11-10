#include <gtest/gtest.h>
#include "UDPClient.hpp"

TEST(UDPClientTest, BasicConstructor) {
    EXPECT_NO_THROW({
        pgw_client::UDPClient client("123456789012345");
    });
}