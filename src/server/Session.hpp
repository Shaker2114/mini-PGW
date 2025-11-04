#pragma once

#include <chrono>

namespace pgw_server
{
    class Session
    {
    public:
        Session(const std::string& imsi);

        std::string getIMSI() const;
        std::chrono::steady_clock::time_point getStartTime() const;

        bool isExpired(int timeout) const;
    private:
        std::string _imsi;
        std::chrono::steady_clock::time_point _start_time;
    };
}