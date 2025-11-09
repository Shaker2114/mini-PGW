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
        auto local_time = std::chrono::current_zone()->to_local(current_time_point); // utc+3, nanosec
        auto local_time_in_sec = std::chrono::time_point_cast<std::chrono::seconds>(local_time); // sec
        std::string timestamp = std::format("{:%d-%m-%Y %H:%M:%S}", local_time_in_sec);
        
        std::string cdr_file_path = "log/" + _filename;

        std::ofstream cdr_file(cdr_file_path, std::ios::app);

        if (cdr_file.is_open()) {
            std::string cdr_string = timestamp + ", " + imsi + ", " + action + "\n";
            cdr_file << cdr_string;
        }
    }
}