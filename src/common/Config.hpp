#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace pgw_common
{
    class Config
    {
    public:
        static nlohmann::json get_config_json(const std::string& file_name);
    };
}
