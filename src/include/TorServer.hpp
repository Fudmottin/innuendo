// src/include/TorServer.hpp

#pragma once
#include "TorInstance.hpp"
#include <string>

class TorServer {
public:
    TorServer(TorInstance& tor, const std::string& service_dir);
    ~TorServer();

    void start();
    void stop();

    std::string onion_address() const;

private:
    TorInstance& tor;
    std::string service_dir;
    std::string onion;
};

