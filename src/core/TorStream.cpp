// src/core/TorStream.cpp

#include "TorStream.hpp"
#include <iostream>  // for logging, if needed

TorStream::TorStream(EventLoop& loop,
                     std::string onion_host,
                     unsigned short onion_port)
    : client_(loop, std::move(onion_host), onion_port),
      onion_host_(std::move(onion_host)),
      onion_port_(onion_port)
{
    // Constructor body empty; all initialization done in initializer list
}

void TorStream::connect() {
    try {
        client_.connect_to_destination(onion_host_, onion_port_);
        std::cout << "[TorStream] Connected to " << onion_host_ 
                  << ":" << onion_port_ << " via SOCKS5 proxy\n";
    } catch (const std::exception& e) {
        std::cerr << "[TorStream] Connection failed: " << e.what() << '\n';
        throw;
    }
}

bool TorStream::connected() const noexcept {
    return client_.is_open();
}

void TorStream::send_line(const std::string& line) {
    client_.write_some(line + "\n");
}

std::string TorStream::read_line(std::size_t max_bytes) {
    return client_.read_some(max_bytes);
}

