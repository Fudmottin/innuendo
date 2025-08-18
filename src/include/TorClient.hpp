// src/include/TorClient.hpp

#pragma once
#include "TorInstance.hpp"
#include <string>
#include <memory>
#include <boost/asio.hpp>

class Socks5Client;

class TorClient {
public:
    TorClient(TorInstance& tor,
              const std::string& server_onion,
              unsigned short server_port = 80);

    ~TorClient();

    void connect();
    void disconnect();

    std::size_t write_some(const std::string& data);
    std::string read_some(std::size_t max_bytes = 0);

private:
    TorInstance& tor_;
    std::string server_onion_;
    unsigned short server_port_;

    boost::asio::io_context ioc_;
    std::unique_ptr<Socks5Client> socks_client_;
};

