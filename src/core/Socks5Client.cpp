// src/core/Socks5Client.cpp

#include "Socks5Client.hpp"
#include <iostream>
#include <system_error>

using boost::asio::ip::tcp;

Socks5Client::Socks5Client(boost::asio::io_context& ioc,
                           std::string socks_host,
                           unsigned short socks_port)
    : ioc_(ioc),
      socket_(ioc),
      socks_host_(std::move(socks_host)),
      socks_port_(socks_port)
{
    tcp::resolver resolver(ioc_);
    auto endpoints = resolver.resolve(socks_host_, std::to_string(socks_port_));
    boost::asio::connect(socket_, endpoints);
}

void Socks5Client::connect_to_destination(const std::string& dest_host, unsigned short dest_port) {
    std::cout << "[Socks5Client] Performing SOCKS5 handshake (no auth)\n";

    unsigned char greeting[3] = {SOCKS_VER, 0x01, SOCKS_METHOD_NOAUTH};
    boost::asio::write(socket_, boost::asio::buffer(greeting));

    unsigned char server_resp[2];
    boost::asio::read(socket_, boost::asio::buffer(server_resp));
    if (server_resp[0] != SOCKS_VER || server_resp[1] != SOCKS_METHOD_NOAUTH) {
        throw std::runtime_error("SOCKS5: handshake rejected or requires auth");
    }

    std::cout << "[Socks5Client] Sending CONNECT for " << dest_host << ":" << dest_port << "\n";

    if (dest_host.size() > MAX_HOSTNAME_LEN)
        throw std::runtime_error("SOCKS5: destination hostname too long");

    std::vector<unsigned char> req;
    req.push_back(SOCKS_VER);                 // VER
    req.push_back(SOCKS_CMD_CONNECT);         // CMD
    req.push_back(SOCKS_RSV);                 // RSV
    req.push_back(SOCKS_ATYP_DOMAIN);         // ATYP
    req.push_back(static_cast<unsigned char>(dest_host.size()));
    req.insert(req.end(), dest_host.begin(), dest_host.end());
    req.push_back(static_cast<unsigned char>((dest_port >> 8) & 0xFF));
    req.push_back(static_cast<unsigned char>(dest_port & 0xFF));

    boost::asio::write(socket_, boost::asio::buffer(req));

    unsigned char reply_head[4];
    boost::asio::read(socket_, boost::asio::buffer(reply_head));

    if (reply_head[0] != SOCKS_VER)
        throw std::runtime_error("SOCKS5: invalid reply version");
    if (reply_head[1] != 0x00)
        throw std::runtime_error("SOCKS5: connection refused or failed (REP != 0)");

    unsigned char atyp = reply_head[3];
    switch (atyp) {
        case SOCKS_ATYP_IPV4: {
            unsigned char skip[4 + 2];
            boost::asio::read(socket_, boost::asio::buffer(skip));
            break;
        }
        case SOCKS_ATYP_DOMAIN: {
            unsigned char len;
            boost::asio::read(socket_, boost::asio::buffer(&len, 1));
            std::vector<unsigned char> skip(len + 2);
            boost::asio::read(socket_, boost::asio::buffer(skip));
            break;
        }
        case SOCKS_ATYP_IPV6: {
            unsigned char skip[16 + 2];
            boost::asio::read(socket_, boost::asio::buffer(skip));
            break;
        }
        default:
            throw std::runtime_error("SOCKS5: unknown ATYP in reply");
    }

    std::cout << "[Socks5Client] SOCKS5 CONNECT successful\n";
}

std::size_t Socks5Client::write_some(const std::string& data) {
    return boost::asio::write(socket_, boost::asio::buffer(data));
}

std::string Socks5Client::read_some(std::size_t max_bytes) {
    if (max_bytes == 0) max_bytes = MAX_READ_BYTES;

    std::vector<char> buf(max_bytes);
    boost::system::error_code ec;
    std::size_t n = socket_.read_some(boost::asio::buffer(buf), ec);
    if (ec && ec != boost::asio::error::eof) {
        throw boost::system::system_error(ec);
    }
    return std::string(buf.data(), n);
}

