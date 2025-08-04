// src/relay/innuendod.cpp

#include <iostream>
#include <csignal>
#include <atomic>

#include "tor/TorInstance.h"

std::atomic<bool> running = true;

void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM)
        running = false;
}

int main(int argc, char** argv) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::cout << "[innuendod] Starting Tor hidden service..." << std::endl;

    // TODO: Implement an innuendo relay

    return 0;
}

