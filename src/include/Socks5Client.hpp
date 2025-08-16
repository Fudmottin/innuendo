#pragma once
#include "EventLoop.hpp"
#include <boost/asio.hpp>
#include <string>
#include <vector>

class Socks5Client {
public:
    Socks5Client(EventLoop& loop, std::string host, unsigned short port) noexcept;
    ~Socks5Client();

    void connect_to_destination(const std::string& dest_host, unsigned short dest_port);
    void write_some(const std::string& data);
    std::string read_some(std::size_t max_bytes);
    bool is_open() const noexcept {
        return socket_.is_open();  // socket_ is a boost::asio::ip::tcp::socket
    }

private:
    EventLoop& loop_;
    boost::asio::ip::tcp::socket socket_;
    std::string socks_host_;
    unsigned short socks_port_;
};

