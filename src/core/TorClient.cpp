// src/core/TorClient.cpp
#include "TorClient.hpp"
#include "Socks5Client.hpp"
#include <iostream>

TorClient::TorClient(TorInstance& tor,
                     const std::string& server_onion,
                     unsigned short server_port)
    : tor_(tor), server_onion_(server_onion), server_port_(server_port) {}

TorClient::~TorClient() {
    disconnect();
}

void TorClient::connect() {
    if (!tor_.is_running())
        throw std::runtime_error("Tor is not running");

    std::cout << "[TorClient] Connecting to " << server_onion_
              << ":" << server_port_ << " via SOCKS5 at "
              << tor_.socks_address() << ":" << tor_.socks_port() << "\n";

    socks_client_ = std::make_unique<Socks5Client>(ioc_, tor_.socks_address(), tor_.socks_port());
    socks_client_->connect_to_destination(server_onion_, server_port_);
}

void TorClient::disconnect() {
    socks_client_.reset();
}

std::size_t TorClient::write_some(const std::string& data) {
    if (!socks_client_) throw std::runtime_error("TorClient not connected");
    return socks_client_->write_some(data);
}

std::string TorClient::read_some(std::size_t max_bytes) {
    if (!socks_client_) throw std::runtime_error("TorClient not connected");
    return socks_client_->read_some(max_bytes);
}

