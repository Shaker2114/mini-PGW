cmake_minimum_required(VERSION 3.20)
project(PGW_Server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# nlohmann::json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)

# cpp-httplib
FetchContent_Declare(
    cpp-httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.26.0
)

# googletest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

# spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.3
)

FetchContent_MakeAvailable(nlohmann_json cpp-httplib googletest spdlog)

# Interface lib for cpp-httplib
add_library(cpp-httplib-interface INTERFACE)
target_include_directories(cpp-httplib-interface INTERFACE 
    ${cpp-httplib_SOURCE_DIR}
)

add_executable(pgw_server src/testfile.cpp)
target_link_libraries(pgw_server PRIVATE
    nlohmann_json::nlohmann_json
    cpp-httplib-interface
    spdlog::spdlog
)
target_include_directories(pgw_server PRIVATE
    ${nlohmann_json_SOURCE_DIR}/include
    ${cpp-httplib_SOURCE_DIR}
    ${googletest_SOURCE_DIR}/googletest/include
    ${spdlog_SOURCE_DIR}/include
)

# enable_testing()

# add_executable(run_tests tests/test_main.cpp tests/test_example.cpp)
# target_link_libraries(run_tests PRIVATE
#     nlohmann_json::nlohmann_json
#     cpp-httplib
#     GTest::gtest_main
# )

# # Добавляем тесты в CTest
# add_test(NAME AllTests COMMAND run_tests)