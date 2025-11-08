#include "PGWServer.hpp"
#include <csignal>

namespace {
    pgw_server::PGWServer* g_pgw_server = nullptr;
}

void signal_handler(int signal) {
    if (g_pgw_server) {
        spdlog::info("Received signal {}, initiating shutdown...", signal);
        g_pgw_server->gracefulShutdown();
    }
}

int main()
{
    try {
        pgw_server::PGWServer main_server;
        
        // signal handler
        g_pgw_server = &main_server;
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);
        
        spdlog::info("Starting PGW Server application...");
        main_server.run();
        
        spdlog::info("PGW Server application finished successfully");
        return 0;

    } catch (const std::exception& ex) {
        spdlog::critical("PGW Server application failed: {}", ex.what());
        return 1;
    } catch (...) {
        spdlog::critical("PGW Server application failed with unknown exception");
        return 1;
    }
}