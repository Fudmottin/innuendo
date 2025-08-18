// src/include/TorInstance.hpp

#pragma once
#include <string>

class TorInstance {
public:
    // Default Tor SOCKS proxy
    static constexpr const char* DEFAULT_SOCKS_HOST = "127.0.0.1";
    static constexpr unsigned short DEFAULT_SOCKS_PORT = 9050;

    TorInstance(std::string socks_host = DEFAULT_SOCKS_HOST,
                unsigned short socks_port = DEFAULT_SOCKS_PORT) noexcept;
    ~TorInstance();

    void start(); // logs what we will do
    void stop();  // logs and tidy up (no-op for now)

    std::string socks_host() const noexcept { return socks_host_; }
    unsigned short socks_port() const noexcept { return socks_port_; }

private:
    std::string socks_host_;
    unsigned short socks_port_;
};

