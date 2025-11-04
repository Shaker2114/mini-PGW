#pragma once

#include <spdlog/spdlog.h>
#include <memory>

namespace pgw_common
{
    class Logger
    {
    public:
        static std::shared_ptr<spdlog::logger> logger_init(const std::string& name,
            const std::string& log_file, const std::string& log_level);
    private:
        static spdlog::level::level_enum getLevelFromString(const std::string& log_level);
    };
}