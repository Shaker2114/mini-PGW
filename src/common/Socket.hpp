#pragma once

#include <nlohmann/json.hpp>
#include <netinet/in.h>
#include <memory>

namespace pgw_common
{
    class Socket
    {
    public:
        static std::unique_ptr<Socket> socket_init(const std::string& ip_addr, int port);
        
        ~Socket();

        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;
        Socket(Socket&&) = default;
        Socket& operator=(Socket&&) = default;

        int getSocketId() const;
        const sockaddr_in& getSocketAddr() const;
    private:
        Socket(int socket_id, const sockaddr_in& addr);

        int _socket_id;
        sockaddr_in _addr;
    };
}