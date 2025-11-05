#include "SessionManager.hpp"

namespace pgw_server
{
    SessionManager::SessionManager(int timeout_sec, const std::string& log_filename)
        : _timeout_sec(timeout_sec), _cdr_writer(log_filename) {}

    bool SessionManager::createSession(const std::string& imsi)
    {
        if (!hasSession(imsi) && !isBlackListed(imsi)) {
            auto emplace_result = _sessions.try_emplace(imsi, std::make_unique<Session>(imsi));
            _cdr_writer.write(imsi, "session_created");
            return emplace_result.second;
        }

        return false;
    }
    bool SessionManager::removeSession(const std::string& imsi)
    {
        if (hasSession(imsi)) {
            auto erase_result = _sessions.erase(imsi);
            return static_cast<bool>(erase_result);
        }

        return false;
    }
    bool SessionManager::hasSession(const std::string& imsi) const
    {
        return _sessions.contains(imsi);
    }

    void SessionManager::checkExpiredSessions()
    {
        for (auto& session : _sessions) {
            if (session.second->isExpired(_timeout_sec)) {
                _cdr_writer.write(session.first, "session_timeout");
                removeSession(session.first);
            }
        }
    }

    void SessionManager::addToBackList(const std::string& imsi)
    {
        _black_list.insert(imsi);
    }
    bool SessionManager::isBlackListed(const std::string& imsi) const
    {
        return _black_list.contains(imsi);
    }

    void SessionManager::shutdownSessions(int graceful_shutdown_rate)
    {
        for (int i = 0; i < graceful_shutdown_rate && _sessions.size() != 0; ++i) {
            auto session_to_remove = _sessions.begin();
            _cdr_writer.write(session_to_remove->first, "session_ended");
            removeSession(session_to_remove->first);
        }
    }

    size_t SessionManager::getSessionsQuantity() const
    {
        return _sessions.size();
    }
}