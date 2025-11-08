#include "Logger.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace pgw_common
{
    std::shared_ptr<spdlog::logger> Logger::logger_init(
        const std::string& name,
        const std::string& log_file,
        const std::string& log_level)
    {
        try {
            std::vector<spdlog::sink_ptr> sinks;
            sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>()); // console_sink
            sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file.c_str(), false)); // file_sink

            auto _logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
            
            _logger->set_level(getLevelFromString(log_level));
            _logger->flush_on(getLevelFromString(log_level));
            _logger->set_pattern("[%d-%m-%Y %H:%M:%S] [%n] [%l] %v");

            _logger->info("{} logger initialized with level: {}", name, log_level);

            return _logger;
        } catch (const std::bad_alloc& ex) {
            throw std::runtime_error("Failed to create " + name + " logger: " + ex.what());
        } catch(const spdlog::spdlog_ex& ex) {
            throw std::runtime_error("Failed to create " + name + " logger: " + ex.what());
        }
    }
    spdlog::level::level_enum Logger::getLevelFromString(const std::string& level)
    {
        if (level == "DEBUG" || level == "debug") return spdlog::level::debug;
        if (level == "INFO" || level == "info") return spdlog::level::info;
        if (level == "WARN" || level == "warn") return spdlog::level::warn;
        if (level == "ERR" || level == "err") return spdlog::level::err;
        if (level == "CRITICAL" || level == "critical") return spdlog::level::critical;

        return spdlog::level::info;
    }
}