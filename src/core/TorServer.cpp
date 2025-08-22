// src/core/TorServer.cpp

#include "TorServer.hpp"
#include <iostream>
#include <fstream>

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

    // Set up local TCP acceptor
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), impl->service_port);
    impl->acceptor.open(ep.protocol());
    impl->acceptor.set_option(boost::asio::socket_base::reuse_address(true));
    impl->acceptor.bind(ep);
    impl->acceptor.listen();

    // Read onion address from Tor hidden service directory
    std::string hostname_path = impl->service_dir + "/hostname";
    std::ifstream hostfile(hostname_path);
    if (hostfile) {
        std::getline(hostfile, impl->onion);
    } else {
        std::cerr << "[TorServer] Warning: could not read "
                  << hostname_path << ". Is Tor configured with HiddenServiceDir?\n";
    }

    impl->running = true;
    std::cout << "[TorServer] Hidden service running at " << impl->onion
              << ":" << impl->service_port << "\n";
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

