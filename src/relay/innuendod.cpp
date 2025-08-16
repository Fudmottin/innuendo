#include "EventLoop.hpp"
#include "TorInstance.hpp"
#include <iostream>

int main() {
    EventLoop loop;

    // Construct TorInstance with default SOCKS proxy
    TorInstance tor(loop, "127.0.0.1", 9050);
    tor.start();

    std::cout << "[Relay] Innuendo relay daemon started.\n";

    // Run the event loop until explicitly stopped
    loop.run();

    std::cout << "[Relay] Exiting.\n";
    return 0;
}

