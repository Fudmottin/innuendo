// src/include/TorServer.hpp

#pragma once
#include "TorInstance.hpp"
#include <boost/asio.hpp>
#include <string>
#include <memory>
#include <functional>

class TorServer {
public:
    using tcp = boost::asio::ip::tcp;
    using AcceptHandler = std::function<void(tcp::socket)>;

    TorServer(boost::asio::io_context& io,
              TorInstance& tor,
              unsigned short port,
              const std::string& service_dir);
    ~TorServer();

    TorServer(const TorServer&) = delete;
    TorServer& operator=(const TorServer&) = delete;

    void start();          // opens acceptor, configures HS
    void stop() noexcept;  // closes acceptor, withdraws HS

    // Start async accept loop: on each connection, call handler with a socket.
    void async_accept(AcceptHandler handler);

    std::string onion_address() const;
    unsigned short port() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

