#include <proxyfmu/client/proxy_fmu.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

using namespace proxyfmu;

TEST_CASE("client_test_executable_next_to_client")
{
    // This test runs in a directory without the proxyfmu executable, and
    // loads the proxyfmu-client library from another directory, which also
    // contains the executable (as when a library embedding proxyfmu is
    // shipped together with it and loaded by an interpreter, e.g. Python).
    // The executable must be found next to the library.
    std::string fmuPath(std::string(REFERENCE_FMUS_DIR) + "/Dahlquist.fmu");
    auto fmu = client::proxy_fmu(fmuPath);
    auto slave = fmu.new_instance("instance");
    REQUIRE(slave->setup_experiment());
    REQUIRE(slave->enter_initialization_mode());
    REQUIRE(slave->exit_initialization_mode());
    REQUIRE(slave->step(0.0, 0.1));
    REQUIRE(slave->terminate());
}
