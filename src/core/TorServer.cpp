// src/core/TorServer.cpp

#include "TorServer.hpp"
#include <stdexcept>

TorServer::TorServer(TorInstance& tor, const std::string& service_dir)
    : tor(tor), service_dir(service_dir) {}

TorServer::~TorServer() {
    stop();
}

void TorServer::start() {
    // TODO: initialize hidden service with TorInstance
    // onion = ...;
}

void TorServer::stop() {
    // TODO: tear down hidden service
}

std::string TorServer::onion_address() const {
    return onion;
}

/*
#include "TorServer.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

TorServer::TorServer(TorInstance& tor,
                     unsigned short virt_port,
                     std::string hs_dir)
    : tor_(tor),
      virt_port_(virt_port),
      hs_dir_(std::move(hs_dir)),
      running_(false) {}

bool TorServer::start() {
    namespace fs = std::filesystem;
    running_ = true;

    try {
        std::cout << "[TorServer] Starting hidden service...\n";
        std::string add_cmd;
        if (!hs_dir_.empty()) {
            fs::path hs_path = hs_dir_;

            if (fs::exists(hs_path / "private_key")) {
                std::ifstream key_file(hs_path / "private_key");
                std::string key((std::istreambuf_iterator<char>(key_file)),
                                std::istreambuf_iterator<char>());
                add_cmd = "ADD_ONION " + key +
                          " Port=" + std::to_string(virt_port_) + "\r\n";
                std::cout << "[TorServer] Reusing existing hidden service key\n";
            } else {
                fs::create_directories(hs_path);
                add_cmd = "ADD_ONION NEW:ED25519-V3 Port=" +
                          std::to_string(virt_port_) + "\r\n";
                std::cout << "[TorServer] Creating new hidden service\n";
            }
        } else {
            add_cmd = "ADD_ONION NEW:ED25519-V3 Port=" +
                      std::to_string(virt_port_) + "\r\n";
            std::cout << "[TorServer] Creating ephemeral hidden service\n";
        }

        tor_.send_control_command(add_cmd, [&](const std::string& line) {
            std::cout << "[TorServer] Tor reply: " << line << "\n";
            if (line.find("ServiceID=") != std::string::npos) {
                onion_address_ = line.substr(line.find("ServiceID=") + 10, 56);
            }
            if (!hs_dir_.empty() && line.find("PrivateKey=") != std::string::npos) {
                std::ofstream key_out(std::filesystem::path(hs_dir_) / "private_key");
                key_out << line.substr(line.find("PrivateKey=") + 11);
            }
        });

        std::cout << "[TorServer] Hidden service ready: "
                  << onion_address_ << ".onion\n";
    } catch (const std::exception& e) {
        std::cerr << "[TorServer] Exception: " << e.what() << "\n";
        return false;
    }

    return true;
}

void TorServer::stop() {
    if (running_) {
        std::cout << "[TorServer] Stopping hidden service (not yet implemented)\n";
        running_ = false;
    }
}
*/

