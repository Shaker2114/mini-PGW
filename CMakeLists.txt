cmake_minimum_required(VERSION 3.20)
project(miniPGW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# std::format
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFMT_HEADER_ONLY")
# static libs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

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

# Common lib
add_library(pgw_common STATIC
    src/common/BCDConverter.cpp
    src/common/Config.cpp
    src/common/Logger.cpp
    src/common/Socket.cpp
)

target_include_directories(pgw_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)

target_link_libraries(pgw_common PUBLIC
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)

# pgw_server
add_executable(pgw_server
    src/server/main_server.cpp
    src/server/PGWServer.cpp
    src/server/UDPServer.cpp
    src/server/HTTPServer.cpp
    src/server/SessionManager.cpp
    src/server/Session.cpp
    src/server/CDRWriter.cpp
)

# Interface lib for cpp-httplib
add_library(cpp-httplib-interface INTERFACE)
target_include_directories(cpp-httplib-interface INTERFACE 
    ${cpp-httplib_SOURCE_DIR}
)

target_include_directories(pgw_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server
    ${cpp-httplib_SOURCE_DIR}
    #${googletest_SOURCE_DIR}/googletest/include
)

target_link_libraries(pgw_server PRIVATE
    pgw_common
    cpp-httplib-interface
    pthread
)

add_executable(pgw_client
    src/client/main_client.cpp
    src/client/UDPClient.cpp
)

target_include_directories(pgw_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client
)

target_link_libraries(pgw_client PRIVATE
    pgw_common
    pthread
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