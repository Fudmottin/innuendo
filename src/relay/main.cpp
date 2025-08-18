// src/relay/main.cpp

#include "TorInstance.hpp"
#include "TorServer.hpp"
#include <iostream>
#include <csignal>

TorServer* g_server = nullptr;

void handle_signal(int) {
    if (g_server) g_server->stop();
    std::exit(0);
}

int main() {
    TorInstance tor;           // Uses 127.0.0.1:9050
    TorServer server(tor, 1440, "/opt/homebrew/var/lib/tor/innuendod");

    g_server = &server;
    std::signal(SIGINT, handle_signal);

    if (!server.start()) return 1;

    std::cout << "[innuendod] Listening on port 1440 (foreground, CTRL-C to exit)\n";

    // Loop to accept messages or just idle
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

