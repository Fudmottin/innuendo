// src/core/TorServer.cpp

// src/core/TorServer.cpp
#include "TorServer.hpp"
#include <iostream>

struct TorServer::Impl {
    boost::asio::io_context& io;
    TorInstance& tor;
    unsigned short service_port;
    std::string service_dir;
    std::string onion = "stub2000.onion"; // TODO: read from Tor service
    tcp::acceptor acceptor;
    bool running = false;

    Impl(boost::asio::io_context& ioc,
         TorInstance& t,
         unsigned short p,
         const std::string& dir)
        : io(ioc), tor(t), service_port(p), service_dir(dir), acceptor(ioc) {}
};

TorServer::TorServer(boost::asio::io_context& io,
                     TorInstance& tor,
                     unsigned short port,
                     const std::string& service_dir)
    : impl(std::make_unique<Impl>(io, tor, port, service_dir)) {}

TorServer::~TorServer() { stop(); }

void TorServer::start() {
    if (impl->running) return;

    std::cout << "[TorServer] Starting hidden service at " << impl->onion
              << ":" << impl->service_port << "\n";

    tcp::endpoint ep(boost::asio::ip::make_address("127.0.0.1"),
                     impl->service_port);
    boost::system::error_code ec;

    impl->acceptor.open(ep.protocol(), ec);
    if (ec) throw std::runtime_error("open failed: " + ec.message());

    impl->acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
    impl->acceptor.bind(ep, ec);
    if (ec) throw std::runtime_error("bind failed: " + ec.message());

    impl->acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec) throw std::runtime_error("listen failed: " + ec.message());

    impl->running = true;
}

void TorServer::stop() noexcept {
    if (!impl->running) return;
    boost::system::error_code ec;
    impl->acceptor.close(ec);
    impl->running = false;
    std::cout << "[TorServer] Hidden service stopped\n";
}

void TorServer::async_accept(AcceptHandler handler) {
    auto sock = std::make_shared<tcp::socket>(impl->io);
    impl->acceptor.async_accept(*sock,
        [this, sock, handler = std::move(handler)]
        (boost::system::error_code ec) mutable {
            if (!ec) {
                handler(std::move(*sock));
            }
            // re-arm accept loop if still running
            if (impl->running && impl->acceptor.is_open())
                this->async_accept(std::move(handler));
        });
}

std::string TorServer::onion_address() const { return impl->onion; }
unsigned short TorServer::port() const { return impl->service_port; }

