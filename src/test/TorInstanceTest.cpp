#include "tor/TorInstance.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TorInstance starts and stops in client mode", "[TorInstance]") {
    innuendo::tor::TorInstance tor(innuendo::tor::TorInstance::Mode::Client);

    REQUIRE(tor.start() == true);

    // You may add more checks here

    tor.stop();
}

