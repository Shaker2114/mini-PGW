#include "SessionManager.hpp"

#include <vector>

namespace pgw_server
{
    SessionManager::SessionManager(
        int timeout_sec,
        const std::string& log_filename,
        const std::shared_ptr<spdlog::logger>& _logger)
        : _timeout_sec(timeout_sec), _cdr_writer(log_filename),
        _svr_logger(_logger) {
            _svr_logger->info("SessionManager created");
        }

    bool SessionManager::createSession(const std::string& imsi)
    {
        if (!hasSession(imsi) && !isBlackListed(imsi)) {
            auto emplace_result = _sessions.try_emplace(imsi, std::make_unique<Session>(imsi));
            
            _cdr_writer.write(imsi, "session_created");
            _svr_logger->info("Session with IMSI={} created", imsi);
            
            return emplace_result.second;
        }

        std::string reason_of_creation_failed = (isBlackListed(imsi)) ?
            "Session's IMSI Blacklisted" :
            "Session already exist";
        
        _svr_logger->info(
            "Session with IMSI={} rejected: {}",
            imsi,
            reason_of_creation_failed
        );
        return false;
    }
    bool SessionManager::removeSession(const std::string& imsi)
    {
        if (_sessions.erase(imsi) > 0) {
            _svr_logger->info("Session with IMSI={} removed", imsi);
            return true;
        }

        return false;
    }
    bool SessionManager::hasSession(const std::string& imsi) const
    {
        return _sessions.contains(imsi);
    }

    void SessionManager::checkExpiredSessions()
    {
        std::vector<std::string> expired_imsis;

        for (const auto& [imsi, session] : _sessions) {
            if (session->isExpired(_timeout_sec)) {
                expired_imsis.push_back(imsi);
            }
        }

        for (const auto& imsi : expired_imsis) {
            _cdr_writer.write(imsi, "session_timeout");
            removeSession(imsi);
        }
    }

    void SessionManager::setBlacklist(const std::vector<std::string>& blacklist)
    {
        _black_list.insert(blacklist.begin(), blacklist.end());
    }

    bool SessionManager::isBlackListed(const std::string& imsi) const
    {
        return _black_list.contains(imsi);
    }

    void SessionManager::shutdownSessions(int graceful_shutdown_rate)
    {
        if (graceful_shutdown_rate <= 0) return;

        std::vector<std::string> sessions_to_remove;
        
        for (auto it = _sessions.begin();
            it != _sessions.end() && sessions_to_remove.size() < graceful_shutdown_rate; 
            ++it) {
            sessions_to_remove.push_back(it->first);
        }
        
        for (const auto& imsi : sessions_to_remove) {
            _cdr_writer.write(imsi, "session_ended");
            removeSession(imsi);
        }
    }

    size_t SessionManager::getSessionsQuantity() const
    {
        return _sessions.size();
    }
}