#pragma once

#include <string>
#include <memory>

namespace innuendo::tor {

class TorInstance {
public:
    enum class Mode {
        Client,
        HiddenService
    };

    explicit TorInstance(Mode mode, std::string dataDir = {});
    ~TorInstance();

    // Start Tor instance (returns immediately, may run in background)
    bool start();

    // Stop Tor instance, blocking until complete
    void stop();

    // Returns onion address if running as HiddenService, empty otherwise
    std::string getOnionAddress() const;

    // Non-copyable, movable
    TorInstance(const TorInstance&) = delete;
    TorInstance& operator=(const TorInstance&) = delete;
    TorInstance(TorInstance&&) noexcept;
    TorInstance& operator=(TorInstance&&) noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace innuendo::tor

