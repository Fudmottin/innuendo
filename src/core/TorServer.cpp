// src/core/TorServer.cpp

#include "TorServer.hpp"
#include <iostream>
#include <thread>

struct TorServer::Impl {
    TorInstance& tor;
    unsigned short service_port;
    std::string service_dir;
    std::string onion = "stub2000.onion"; // stub, or read from file
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::acceptor acceptor{ioc};
    bool running = false;

    Impl(TorInstance& t, unsigned short p, const std::string& dir)
        : tor(t), service_port(p), service_dir(dir) {}
};

TorServer::TorServer(TorInstance& tor, unsigned short port, const std::string& service_dir)
    : impl(std::make_unique<Impl>(tor, port, service_dir)) {}

TorServer::~TorServer() { stop(); }

void TorServer::start() {
    if (impl->running) return;

    std::cout << "[TorServer] Starting hidden service at " << impl->onion
              << ":" << impl->service_port << "\n";

    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), impl->service_port);
    impl->acceptor.open(ep.protocol());
    impl->acceptor.set_option(boost::asio::socket_base::reuse_address(true));
    impl->acceptor.bind(ep);
    impl->acceptor.listen();

    impl->running = true;
}

void TorServer::stop() {
    if (!impl->running) return;
    boost::system::error_code ec;
    impl->acceptor.close(ec);
    impl->running = false;
    std::cout << "[TorServer] Hidden service stopped\n";
}

std::optional<boost::asio::ip::tcp::socket> TorServer::accept_connection() {
    if (!impl->running) return std::nullopt;
    boost::asio::ip::tcp::socket sock(impl->ioc);
    boost::system::error_code ec;
    impl->acceptor.accept(sock, ec);
    if (ec) return std::nullopt;
    return sock;
}

std::string TorServer::onion_address() const { return impl->onion; }
unsigned short TorServer::port() const { return impl->service_port; }

