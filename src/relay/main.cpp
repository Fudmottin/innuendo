// src/relay/main.cpp

// src/relay/main.cpp
#include "TorInstance.hpp"
#include "TorServer.hpp"

#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <memory>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

// Echo session
class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket socket)
        : socket_(std::move(socket)),
          strand_(socket_.get_executor()) {}

    void start() { do_read(); }

private:
    void do_read() {
        auto self = shared_from_this();
        socket_.async_read_some(
            asio::buffer(buf_),
            asio::bind_executor(strand_,
                [self](boost::system::error_code ec, std::size_t n) {
                    if (!ec) self->do_write(n);
                }));
    }

    void do_write(std::size_t n) {
        auto self = shared_from_this();
        asio::async_write(
            socket_, asio::buffer(buf_.data(), n),
            asio::bind_executor(strand_,
                [self](boost::system::error_code ec, std::size_t /*bytes*/) {
                    if (!ec) self->do_read();
                }));
    }

    tcp::socket socket_;
    asio::strand<asio::any_io_executor> strand_;
    std::array<char, 4096> buf_{};
};

int main() try {
    asio::io_context io;
    auto guard = asio::make_work_guard(io);

    // Start Tor
    TorInstance tor;
    tor.start();

    // Start hidden service
    TorServer server(io, tor, 2000, "/opt/homebrew/var/lib/tor/innuendod");
    server.start();

    std::cout << "[main] Echo server running at "
              << server.onion_address() << ":" << server.port()
              << " (Ctrl-C to quit)\n";

    // Accept loop
    server.async_accept([&](tcp::socket sock) {
        std::make_shared<Session>(std::move(sock))->start();
    });

    // Handle Ctrl-C / SIGTERM
    asio::signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait([&](const boost::system::error_code&, int) {
        std::cout << "\n[main] Signal received, shutting down...\n";
        server.stop();
        tor.stop();
        guard.reset(); // let io.run() exit
        io.stop();
    });

    // Thread pool
    const unsigned n = std::max(1u, std::thread::hardware_concurrency());
    std::vector<std::thread> pool;
    pool.reserve(n);
    for (unsigned i = 0; i < n; ++i)
        pool.emplace_back([&]{ io.run(); });

    for (auto& t : pool) t.join();
    std::cout << "[main] Clean exit.\n";
    return 0;

} catch (const std::exception& e) {
    std::cerr << "[main] Fatal error: " << e.what() << '\n';
    return 1;
}

