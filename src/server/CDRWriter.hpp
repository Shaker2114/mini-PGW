#pragma once

#include <string>

namespace pgw_server
{
    class CDRWriter
    {
    public:
        explicit CDRWriter(const std::string& filename);
        void write(const std::string& imsi, const std::string& action);
    private:
        std::string _filename;
    };
}