#include "Socket.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <unistd.h>

namespace pgw_common
{
    std::unique_ptr<Socket> Socket::socket_init(const std::string& ip_addr, int port)
    {
        int socket_id = socket(AF_INET, SOCK_DGRAM, 0);

        if (socket_id < 0) {
            throw std::runtime_error("Failed to create socket with socket()");
        }
        
        try {
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);

            if (inet_pton(AF_INET, ip_addr.c_str(), &addr.sin_addr) <= 0) {
                throw std::runtime_error("Invalid IP address: " + ip_addr);
            }
            
            if (bind(socket_id, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
                throw std::runtime_error("bind() failed on " + ip_addr + ":" + std::to_string(port));
            }

            return std::unique_ptr<Socket>(new Socket(socket_id, addr));
        } catch(const std::exception& ex) {
            close(socket_id);
            throw std::runtime_error("Failed to initialize socket: " + std::string(ex.what()));
        }
    }
    
    Socket::Socket(int socket_id, const sockaddr_in& addr)
        : _socket_id(socket_id), _addr(addr) {}
    
    Socket::~Socket()
    {
        close(_socket_id);
    }
    
    int Socket::getSocketId() const
    {
        return _socket_id;
    }
    const sockaddr_in& Socket::getSocketAddr() const
    {
        return _addr;
    }
}