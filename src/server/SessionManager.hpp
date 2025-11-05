#pragma once

#include "Session.hpp"
#include <unordered_map>
#include <unordered_set>
#include <memory>
namespace pgw_server
{
    class SessionManager
    {
    public:
        SessionManager(int timeout_sec);

        bool createSession(const std::string& imsi);
        bool removeSession(const std::string& imsi);
        bool hasSession(const std::string& imsi) const;
        
        void checkExpiredSessions();

        void addToBackList(const std::string& imsi);
        bool isBlackListed(const std::string& imsi) const;

        void shutdownSessions(int graceful_shutdown_rate);

        size_t getSessionsQuantity() const;
    private:
        std::unordered_map<std::string, std::unique_ptr<Session>> _sessions;
        std::unordered_set<std::string> _black_list;
        int _timeout_sec;
        // mutable std::mutex _mutex;
    };
}