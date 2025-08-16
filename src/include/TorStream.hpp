// src/include/TorStream.hpp

#pragma once

#include <string>
#include "Socks5Client.hpp"
#include "EventLoop.hpp"

class TorStream {
public:
    TorStream(EventLoop& loop, std::string host, unsigned short port);

    void connect();
    bool connected() const noexcept;
    void send_line(const std::string& line);
    std::string read_line(std::size_t max_bytes = 4096);

private:
    Socks5Client client_;
    std::string onion_host_;
    unsigned short onion_port_;
};

