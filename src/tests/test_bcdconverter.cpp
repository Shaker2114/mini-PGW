#include <gtest/gtest.h>
#include "BCDConverter.hpp"
#include <vector>
#include <stdexcept>

TEST(BCDConverterTest, ValidIMSI) {
    EXPECT_TRUE(pgw_common::BCDConverter::is_valid_imsi("123456789012345"));
    EXPECT_TRUE(pgw_common::BCDConverter::is_valid_imsi("001010000000001"));
    EXPECT_TRUE(pgw_common::BCDConverter::is_valid_imsi("999999999999999"));
    EXPECT_TRUE(pgw_common::BCDConverter::is_valid_imsi("413412"));
    EXPECT_TRUE(pgw_common::BCDConverter::is_valid_imsi("123456789012"));
    EXPECT_TRUE(pgw_common::BCDConverter::is_valid_imsi("3482347892748"));
}

TEST(BCDConverterTest, InvalidIMSI) {
    EXPECT_FALSE(pgw_common::BCDConverter::is_valid_imsi(""));

    EXPECT_FALSE(pgw_common::BCDConverter::is_valid_imsi("123abc456"));
    EXPECT_FALSE(pgw_common::BCDConverter::is_valid_imsi("12.3456789"));
    EXPECT_FALSE(pgw_common::BCDConverter::is_valid_imsi("12345 6789"));

    EXPECT_FALSE(pgw_common::BCDConverter::is_valid_imsi("1234567890123456"));
    EXPECT_FALSE(pgw_common::BCDConverter::is_valid_imsi("12345678901234567890"));
}

TEST(BCDConverterTest, RoundTripEvenLength) {
    std::string imsi = "1234567890";  // чётная длина
    auto bcd = pgw_common::BCDConverter::imsi_to_bcd(imsi);
    auto result = pgw_common::BCDConverter::bcd_to_imsi(bcd);
    EXPECT_EQ(imsi, result);
}

TEST(BCDConverterTest, RoundTripOddLength) {
    std::string imsi = "123456789";   // нечётная длина
    auto bcd = pgw_common::BCDConverter::imsi_to_bcd(imsi);
    auto result = pgw_common::BCDConverter::bcd_to_imsi(bcd);
    EXPECT_EQ(imsi, result);
}

TEST(BCDConverterTest, RoundTripMaxLength) {
    std::string imsi = "123456789012345";  // макс длина 15
    auto bcd = pgw_common::BCDConverter::imsi_to_bcd(imsi);
    auto result = pgw_common::BCDConverter::bcd_to_imsi(bcd);
    EXPECT_EQ(imsi, result);
}

TEST(BCDConverterTest, BCDToIMSIConversion) {
    // IMSI "12" в BCD: 00010010 → 0x12
    std::vector<uint8_t> bcd_data = {0x12};
    std::string result = pgw_common::BCDConverter::bcd_to_imsi(bcd_data);
    EXPECT_EQ("12", result);
}

TEST(BCDConverterTest, BCDToIMSIOddLength) {
    // IMSI "123" в BCD: 0x12 0x3F (с заполнением)
    std::vector<uint8_t> bcd_data = {0x12, 0x3F};
    std::string result = pgw_common::BCDConverter::bcd_to_imsi(bcd_data);
    EXPECT_EQ("123", result);
}

TEST(BCDConverterTest, InvalidBCDInput) {
    std::vector<uint8_t> empty_bcd;
    EXPECT_THROW(
        pgw_common::BCDConverter::bcd_to_imsi(empty_bcd), 
        std::invalid_argument
    );
}

TEST(BCDConverterTest, InvalidIMSIInput) {
    EXPECT_THROW(
        pgw_common::BCDConverter::imsi_to_bcd("12abc"), 
        std::invalid_argument
    );
    
    EXPECT_THROW(
        pgw_common::BCDConverter::imsi_to_bcd(""), 
        std::invalid_argument
    );

    EXPECT_THROW(
        pgw_common::BCDConverter::imsi_to_bcd("123456789012345667890"), 
        std::invalid_argument
    );
}
