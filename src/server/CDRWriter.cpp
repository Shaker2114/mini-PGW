#include "CDRWriter.hpp"

#include <fstream>
#include <chrono>

namespace pgw_server
{
    CDRWriter::CDRWriter(const std::string& filename)
        : _filename(filename) {}
    
    void CDRWriter::write(const std::string& imsi, const std::string& action)
    {
        auto current_time_point = std::chrono::system_clock::now(); // utc
        current_time_point += std::chrono::hours(3); // utc+3
        auto time_in_seconds = std::chrono::time_point_cast<std::chrono::seconds>(current_time_point); // in seconds
        std::string timestamp = std::format("{:%d-%m-%Y %H:%M:%S}", time_in_seconds);
        
        std::ofstream cdr_file(_filename, std::ios::app);
        
        if (cdr_file.is_open()) {
            std::string cdr_string = timestamp + ", " + imsi + ", " + action + "\n";
            
            cdr_file << cdr_string;
            
            cdr_file.close();
        }
    }
}