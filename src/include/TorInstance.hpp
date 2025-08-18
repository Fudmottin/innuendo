// src/include/TorInstance.hpp

#pragma once
#include <string>
#include <memory>

class TorInstance {
public:
    // optional host and port
    TorInstance(const std::string& host = "127.0.0.1", int port = 9050);
    ~TorInstance();

    TorInstance(const TorInstance&) = delete;
    TorInstance& operator=(const TorInstance&) = delete;

    void start();   // bootstrap Tor
    void stop();    // clean shutdown

    bool is_running() const noexcept;

    // Generic interface: Tor paths, socks address, etc.
    std::string socks_address() const;
    int socks_port() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

