#include "BCDConverter.hpp"

#include <stdexcept>

namespace pgw_common
{
    std::vector<uint8_t> BCDConverter::imsi_to_bcd(const std::string& imsi)
    {
        if (!is_valid_imsi(imsi)) {
            throw std::invalid_argument("Invalid IMSI: must contain only digits");
        }

        size_t imsi_len = imsi.length();
        size_t bcd_len = (imsi_len + 1) / 2;
        std::vector<uint8_t> bcd_data(bcd_len, 0);

        // group by 2 digits (2 digits is 1 byte)
        for (size_t i = 0; i < imsi_len; ++i)
        {
            uint8_t digit = imsi[i] - '0';

            if (i % 2 == 0) {
                // first half of byte
                bcd_data[i/2] = digit << 4;
            }
            else {
                // second half of byte
                bcd_data[i/2] |= digit;
            }
        }

        if (imsi_len % 2 != 0) {
            bcd_data.back() |= 0b1111;
        }

        return bcd_data;
    }

    std::string BCDConverter::bcd_to_imsi(const std::vector<uint8_t>& bcd_data)
    {
        if (bcd_data.empty()) {
            throw std::invalid_argument("BCD data cannot be empty");
        }

        std::string imsi;

        for (size_t i = 0; i < bcd_data.size(); ++i)
        {
            uint8_t bcd_number = bcd_data[i];

            char first_imsi_digit = (bcd_number >> 4) + '0';
            imsi.push_back(first_imsi_digit);
            // check on 0b1111(15), 15 > 9 => skip
            if (uint8_t second_digit = (bcd_number & 0b1111); second_digit <= 9) {
                char second_imsi_digit = second_digit + '0';
                imsi.push_back(second_imsi_digit);
            }
            
        }

        return imsi;
    }

    bool BCDConverter::is_valid_imsi(const std::string& imsi)
    {
        if (imsi.empty() || imsi.size() > 15) {
            return false;
        }
        
        for (char ch : imsi) {
            if (!isdigit(ch)) {
                return false;
            }
        }

        return true;
    }
}