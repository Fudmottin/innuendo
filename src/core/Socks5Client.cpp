#include "Socks5Client.hpp"
#include <iostream>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

Socks5Client::Socks5Client(EventLoop& loop, std::string host, unsigned short port) noexcept
    : loop_(loop),
      socket_(loop.get_io_context()),
      socks_host_(std::move(host)),
      socks_port_(port)
{}

Socks5Client::~Socks5Client() {
    boost::system::error_code ec;
    socket_.close(ec);
}

void Socks5Client::connect_to_destination(const std::string& dest_host, unsigned short dest_port) {
    boost::asio::ip::tcp::resolver resolver(loop_.get_io_context());
    auto endpoints = resolver.resolve(socks_host_, std::to_string(socks_port_));
    boost::asio::connect(socket_, endpoints);

    // For now: assume SOCKS5 negotiation is handled externally or transparently
    std::cout << "[Socks5Client] Connected to " << socks_host_ << ":" << socks_port_ << "\n";
}

void Socks5Client::write_some(const std::string& data) {
    boost::asio::write(socket_, boost::asio::buffer(data));
}

std::string Socks5Client::read_some(std::size_t max_bytes) {
    std::vector<char> buf(max_bytes);
    boost::system::error_code ec;
    std::size_t n = socket_.read_some(boost::asio::buffer(buf), ec);
    if (ec) {
        return {};
    }
    return std::string(buf.data(), n);
}

