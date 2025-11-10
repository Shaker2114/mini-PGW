#include <gtest/gtest.h>
#include "CDRWriter.hpp"
#include <filesystem>

TEST(CDRWriterTest, CreateCDRWriter) {
    EXPECT_NO_THROW({
        pgw_server::CDRWriter writer("test_cdr1.log");
    });
}

TEST(CDRWriterTest, WriteCDREntry) {
    pgw_server::CDRWriter writer("test_cdr2.log");
    
    EXPECT_NO_THROW({
        writer.write("123456789012345", "session_created");
    });
    
    EXPECT_TRUE(std::filesystem::exists("log/test_cdr2.log"));
}

TEST(CDRWriterTest, WriteMultipleEntries) {
    pgw_server::CDRWriter writer("test_cdr3.log");
    
    writer.write("111111111111111", "session_created");
    writer.write("222222222222222", "session_created");
    writer.write("111111111111111", "session_ended");
    
    EXPECT_TRUE(std::filesystem::exists("log/test_cdr3.log"));
}

// Cleanup после всех тестов
TEST(CDRWriterTest, CleanupTestFiles) {
    std::filesystem::remove("log/test_cdr1.log");
    std::filesystem::remove("log/test_cdr2.log");
    std::filesystem::remove("log/test_cdr3.log");
}