#pragma once
#include <string>
#include <iostream>
#include "EventLoop.hpp"

class TorInstance {
public:
    TorInstance(EventLoop& loop, std::string socks_host, unsigned short socks_port) noexcept;
    ~TorInstance();

    void start();
    void stop();

private:
    EventLoop& loop_;
    std::string socks_host_;
    unsigned short socks_port_;
    bool running_ = false;
};

