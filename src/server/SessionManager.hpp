#pragma once

#include "Session.hpp"
#include "CDRWriter.hpp"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <spdlog/spdlog.h>

namespace pgw_server
{
    class SessionManager
    {
    public:
        SessionManager(
            int timeout_sec,
            const std::string& log_filename,
            const std::shared_ptr<spdlog::logger>& _logger);

        bool createSession(const std::string& imsi);
        bool removeSession(const std::string& imsi);
        bool hasSession(const std::string& imsi) const;

        void checkExpiredSessions();

        void setBlacklist(const std::vector<std::string>& blacklist);
        bool isBlackListed(const std::string& imsi) const;

        void shutdownSessions(int graceful_shutdown_rate);

        size_t getSessionsQuantity() const;
    private:
        std::unordered_map<std::string, std::unique_ptr<Session>> _sessions;
        std::unordered_set<std::string> _black_list;
        std::shared_ptr<spdlog::logger> _svr_logger;
        CDRWriter _cdr_writer;
        int _timeout_sec;
    };
}