#include "Session.hpp"

namespace pgw_server
{
    Session::Session(const std::string& imsi)
        : _imsi(imsi),_start_time(std::chrono::steady_clock::now())
    {}
    
    std::string Session::getIMSI() const
    {
        return _imsi;
    }
    std::chrono::steady_clock::time_point Session::getStartTime() const
    {
        return _start_time;
    }
    bool Session::isExpired(int timeout) const
    {
        auto current_time = std::chrono::steady_clock::now();
        auto time_from_start = std::chrono::steady_clock::duration(current_time - _start_time);
        
        return time_from_start > static_cast<float>(timeout);
    }
}