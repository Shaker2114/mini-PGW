#include "UDPClient.hpp"
#include <csignal>

namespace {
    pgw_client::UDPClient* g_pgw_client = nullptr;
}

void signal_handler(int signal) {
    if (g_pgw_client) {
        spdlog::info("Received signal {}, client shutdown...", signal);
    }
}

int main(int argc, char* argv[])
{
    try {
        if (argc != 2) {
            throw std::runtime_error("There should be only 1 additional command line argument");
        }

        pgw_client::UDPClient client(argv[1]);

        // signal handler
        g_pgw_client = &client;
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        client.configure();
        client.serverInteraction();
        return 0;

    } catch(const std::exception& ex) {
        spdlog::critical("The Client application failed: {}", ex.what());
        return 1;
    } catch(...) {
        spdlog::critical("The Client application failed with unknown exception");
        return 1;
    }
}