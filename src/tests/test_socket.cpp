#include <gtest/gtest.h>
#include "Socket.hpp"
#include <stdexcept>
#include <string>

TEST(SocketTest, CreateSocketWithValidParameters) {
    // Тестируем создание сокета с валидными параметрами
    EXPECT_NO_THROW({
        auto socket = pgw_common::Socket::socket_init("Client", "127.0.0.1", 9000);
        EXPECT_NE(socket, nullptr);
        EXPECT_GT(socket->getSocketId(), 0);  // socket ID должен быть > 0
    });
}

TEST(SocketTest, CreateServerSocket) {
    // Тестируем создание серверного сокета
    EXPECT_NO_THROW({
        auto socket = pgw_common::Socket::socket_init("Server", "0.0.0.0", 9000);
        EXPECT_NE(socket, nullptr);
        EXPECT_GT(socket->getSocketId(), 0);
    });
}

TEST(SocketTest, InvalidEntityName) {
    // Невалидное имя entity
    EXPECT_THROW({
        auto socket = pgw_common::Socket::socket_init("InvalidEntity", "127.0.0.1", 9000);
    }, std::runtime_error);
}

TEST(SocketTest, InvalidIPAddress) {
    // Невалидный IP адрес
    EXPECT_THROW({
        auto socket = pgw_common::Socket::socket_init("Client", "999.999.999.999", 9000);
    }, std::runtime_error);
}

TEST(SocketTest, CaseInsensitiveEntityNames) {
    // Регистронезависимые имена entity
    EXPECT_NO_THROW({
        auto socket1 = pgw_common::Socket::socket_init("client", "127.0.0.1", 9002);
        auto socket2 = pgw_common::Socket::socket_init("Client", "127.0.0.1", 9003);
        auto socket3 = pgw_common::Socket::socket_init("server", "0.0.0.0", 9004);
        auto socket4 = pgw_common::Socket::socket_init("Server", "0.0.0.0", 9005);
        
        EXPECT_NE(socket1, nullptr);
        EXPECT_NE(socket2, nullptr);
        EXPECT_NE(socket3, nullptr);
        EXPECT_NE(socket4, nullptr);
    });
}

TEST(SocketTest, DifferentPortNumbers) {
    // Разные порты (кроме зарезервированных)
    EXPECT_NO_THROW({
        auto socket1 = pgw_common::Socket::socket_init("Client", "127.0.0.1", 10000);
        auto socket2 = pgw_common::Socket::socket_init("Client", "127.0.0.1", 20000);
        auto socket3 = pgw_common::Socket::socket_init("Client", "127.0.0.1", 30000);
        
        EXPECT_NE(socket1, nullptr);
        EXPECT_NE(socket2, nullptr);
        EXPECT_NE(socket3, nullptr);
    });
}

TEST(SocketTest, LocalhostAndAnyAddress) {
    // Разные валидные IP адреса
    EXPECT_NO_THROW({
        auto socket1 = pgw_common::Socket::socket_init("Client", "127.0.0.1", 9006);
        auto socket2 = pgw_common::Socket::socket_init("Client", "192.168.0.0", 9007);
        auto socket3 = pgw_common::Socket::socket_init("Server", "0.0.0.0", 9008);
        
        EXPECT_NE(socket1, nullptr);
        EXPECT_NE(socket2, nullptr);
        EXPECT_NE(socket3, nullptr);
    });
}

TEST(SocketTest, SocketMoveOperations) {
    // Проверяем move семантику
    auto socket1 = pgw_common::Socket::socket_init("Client", "127.0.0.1", 9009);
    EXPECT_NE(socket1, nullptr);
    
    int original_socket_id = socket1->getSocketId();
    
    // Move конструктор
    auto socket2 = std::move(socket1);
    EXPECT_EQ(socket1, nullptr);  // исходный теперь nullptr
    EXPECT_NE(socket2, nullptr);
    EXPECT_EQ(socket2->getSocketId(), original_socket_id);
}

TEST(SocketTest, GetSocketAddress) {
    // Проверяем что адрес возвращается корректно
    auto socket = pgw_common::Socket::socket_init("Client", "127.0.0.1", 9010);
    EXPECT_NE(socket, nullptr);
    
    const auto& addr = socket->getSocketAddr();
    EXPECT_EQ(addr.sin_family, AF_INET);
    EXPECT_EQ(addr.sin_port, htons(9010));
}
