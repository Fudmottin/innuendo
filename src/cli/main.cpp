// src/cli/main.cpp

#include "EventLoop.hpp"
#include "TorStream.hpp"
#include <iostream>
#include <exception>

int main() {
    EventLoop loop;
    TorStream tor(loop, "nb5iaajanzfty33y2tkuengk37gbjjeylr5gtwnkhkxkrls6k33gvkad.onion", 2000);

    tor.connect();

    if (tor.connected()) {
        tor.send_line("Hello Tor world!");
        std::string reply = tor.read_line(1024);
        std::cout << "Received: " << reply << '\n';
    }
}

