// src/include/Socks5Client.hpp

#pragma once
#include <boost/asio.hpp>
#include <string>
#include <vector>

class Socks5Client {
public:
    Socks5Client(boost::asio::io_context& ioc,
                 std::string socks_host,
                 unsigned short socks_port);

    void connect_to_destination(const std::string& dest_host, unsigned short dest_port);

    std::size_t write_some(const std::string& data);
    std::string read_some(std::size_t max_bytes = MAX_READ_BYTES);

private:
    boost::asio::io_context& ioc_;
    boost::asio::ip::tcp::socket socket_;
    std::string socks_host_;
    unsigned short socks_port_;

    // SOCKS5 constants
    static constexpr unsigned char SOCKS_VER = 0x05;
    static constexpr unsigned char SOCKS_METHOD_NOAUTH = 0x00;
    static constexpr unsigned char SOCKS_CMD_CONNECT = 0x01;
    static constexpr unsigned char SOCKS_ATYP_IPV4 = 0x01;
    static constexpr unsigned char SOCKS_ATYP_DOMAIN = 0x03;
    static constexpr unsigned char SOCKS_ATYP_IPV6 = 0x04;
    static constexpr unsigned char SOCKS_RSV = 0x00;
    static constexpr std::size_t MAX_HOSTNAME_LEN = 255;
    static constexpr std::size_t MAX_READ_BYTES = 4096;
};

