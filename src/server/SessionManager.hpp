#pragma once

#include "Session.hpp"
#include <unordered_map>
#include <unordered_set>
namespace pgw_server
{
    class SessionManager
    {
    public:
        SessionManager(int timeout_sec);

        bool createSession(const std::string& imsi);
        bool removeSession(const std::string& imsi);
        bool hasSession(const std::string& imsi) const;
        
        void checkExpiredSessions(); // внутри проверить кажду сессию: session.isExpired();

        void addToBackList(const std::string& imsi);
        bool inBlackListed(const std::string& imsi) const;

        void shutdownSessions(int graceful_shutdown_rate);
    private:
        std::unordered_map<std::string, Session> _sessions; // <std::string, std::unique_ptr<Session>>
        std::unordered_set<std::string> _black_list;
        int _timeout_sec;
        // std::shared_ptr<spdlog::logger> _logger; - ?
        // mutable std::mutex _mutex;
    };
}