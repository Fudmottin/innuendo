// src/core/TorInstance.cpp

#include "TorInstance.hpp"
#include <iostream>

struct TorInstance::Impl {
    bool running = false;
    std::string host;
    int port;

    Impl(std::string h, int p) : host(std::move(h)), port(p) {}
};

TorInstance::TorInstance(const std::string& host, int port)
    : impl(std::make_unique<Impl>(host, port)) {}

TorInstance::~TorInstance() { stop(); }

void TorInstance::start() {
    if (!impl->running) {
        std::cout << "[TorInstance] Starting Tor (stub) at " 
                  << impl->host << ":" << impl->port << "...\n";
        // TODO: real bootstrap logic
        impl->running = true;
    }
}

void TorInstance::stop() {
    if (impl->running) {
        std::cout << "[TorInstance] Stopping Tor (stub)...\n";
        impl->running = false;
    }
}

bool TorInstance::is_running() const noexcept {
    return impl->running;
}

std::string TorInstance::socks_address() const {
    return impl->host;
}

int TorInstance::socks_port() const {
    return impl->port;
}

