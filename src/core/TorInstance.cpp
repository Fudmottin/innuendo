#include "TorInstance.hpp"

TorInstance::TorInstance(EventLoop& loop, std::string socks_host, unsigned short socks_port) noexcept
    : loop_(loop), socks_host_(std::move(socks_host)), socks_port_(socks_port) {}

TorInstance::~TorInstance() {
    stop();
}

void TorInstance::start() {
    if (running_) return;
    running_ = true;

    std::cout << "[TorInstance] Using SOCKS proxy " 
              << socks_host_ << ":" << socks_port_ 
              << " (assumes Tor daemon is running)\n";

    // Here you would normally initiate circuit setup, etc.
}

void TorInstance::stop() {
    if (!running_) return;
    running_ = false;

    std::cout << "[TorInstance] Stopping (no action performed)\n";

    // Here you would normally clean up connections
}

