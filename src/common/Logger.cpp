#include "Logger.hpp"

#include <spdlog/sinks/basic_file_sink.h>
// #include <spdlog/sinks/stdout_color_sinks.h>

namespace pgw_common
{
    std::shared_ptr<spdlog::logger> Logger::logger_init(
        const std::string& name,
        const std::string& log_file,
        const std::string& log_level)
    {
        try {
            spdlog::sink_ptr file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file.c_str(), true);
                
            auto _logger = std::make_shared<spdlog::logger>(name, file_sink);
            
            _logger->set_level(getLevelFromString(log_level));
            
            _logger->info("{}_logger initialized with level: {}", name, log_level);

            return _logger;
        } catch (const std::bad_alloc& ex) {
            throw std::runtime_error("Failed to create " + name + "_logger: " + ex.what());
        } catch(const spdlog::spdlog_ex& ex) {
            throw std::runtime_error("Failed to create " + name + "_logger: " + ex.what());
        }
    }
    spdlog::level::level_enum Logger::getLevelFromString(const std::string& level)
    {
        if (level == "DEBUG") return spdlog::level::debug;
        if (level == "INFO") return spdlog::level::info;
        if (level == "WARN") return spdlog::level::warn;
        if (level == "ERR") return spdlog::level::err;
        if (level == "CRITICAL") return spdlog::level::critical;

        return spdlog::level::info;
    }
}