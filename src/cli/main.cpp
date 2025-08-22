// src/cli/main.cpp

#include "TorInstance.hpp"
#include "TorClient.hpp"

#include <chrono>
#include <iostream>
#include <optional>
#include <string>

#ifdef HAVE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#endif

static std::optional<std::string> get_input(const char* prompt) {
#ifdef HAVE_READLINE
    char* buf = readline(prompt);
    if (!buf) return std::nullopt;
    std::string s(buf);
    free(buf);
    if (!s.empty()) add_history(s.c_str());
    return s;
#else
    std::cout << prompt << std::flush;
    std::string s;
    if (!std::getline(std::cin, s)) return std::nullopt;
    return s;
#endif
}

int main() try {
    const std::string onion =
        "qyhw7f6jynynrcyffeploro2bnbssab3ed5few2nb3joj2sap2vinsid.onion";
    const unsigned short onion_port = 1440;

    TorInstance tor;
    tor.start();
    TorClient client(tor, onion, onion_port);

    std::cout << "[main] Establishing circuit and connecting to onion...\n";
    client.connect();
    std::cout << "[main] Connected. Type text and press Enter to send. Type 'quit' to exit.\n";

    while (true) {
        auto maybe = get_input("tor-echo> ");
        if (!maybe) {
            std::cout << "\n[main] EOF, exiting.\n";
            break;
        }
        const std::string line = *maybe;
        if (line == "quit") break;
        if (line.empty()) continue;

        const auto t0 = std::chrono::steady_clock::now();
        client.write_some(line + "\n");
        const std::string reply = client.read_some();
        const auto t1 = std::chrono::steady_clock::now();

        std::chrono::duration<double> rtt = t1 - t0;
        std::cout << "Echo: " << reply << " (" << rtt.count() << " s)\n";
    }

    tor.stop();
    return 0;
}
catch (const std::exception& e) {
    std::cerr << "[ERROR] " << e.what() << '\n';
    return 2;
}

