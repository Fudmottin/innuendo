// src/relay/main.cpp

#include "TorInstance.hpp"
#include "TorServer.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <csignal>

volatile std::sig_atomic_t g_quit = 0;
void signal_handler(int) { g_quit = 1; }

int main() {
    std::signal(SIGINT, signal_handler);

    TorInstance tor;
    tor.start();

    TorServer server(tor, 2000, "/opt/homebrew/var/lib/tor/innuendod");
    server.start();

    std::cout << "[main] Echo server running at "
              << server.onion_address() << ":" << server.port()
              << " (Ctrl-C to quit)\n";

    while (!g_quit) {
        auto sock_opt = server.accept_connection();
        if (sock_opt) {
            auto& sock = *sock_opt;
            std::vector<char> buf(4096);
            boost::system::error_code ec;
            std::size_t n = sock.read_some(boost::asio::buffer(buf), ec);
            if (!ec && n > 0)
                boost::asio::write(sock, boost::asio::buffer(buf.data(), n));
        }
    }

    server.stop();
    tor.stop();
}

