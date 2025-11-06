#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace pgw_common
{
    class BCDConverter
    {
    public:
        static std::vector<uint8_t> imsi_to_bcd(const std::string& imsi);

        static std::string bcd_to_imsi(const std::vector<uint8_t>& bcd_data);

        static bool is_valid_imsi(const std::string& imsi);
    };
}