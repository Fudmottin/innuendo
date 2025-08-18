// src/include/TorServer.hpp

#pragma once
#include "TorInstance.hpp"
#include <boost/asio.hpp>
#include <string>
#include <optional>

class TorServer {
public:
    TorServer(TorInstance& tor, unsigned short port, const std::string& service_dir);
    ~TorServer();

    TorServer(const TorServer&) = delete;
    TorServer& operator=(const TorServer&) = delete;

    void start();  // starts hidden service, creates acceptor
    void stop();   // stops acceptor and service

    // Blocking accept
    std::optional<boost::asio::ip::tcp::socket> accept_connection();

    std::string onion_address() const;
    unsigned short port() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

