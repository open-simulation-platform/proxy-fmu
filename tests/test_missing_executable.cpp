#include <proxyfmu/client/proxy_fmu.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

using namespace proxyfmu;

TEST_CASE("client_test_missing_executable")
{
    // This test runs where there is no proxyfmu executable, so the proxy
    // process can't be started. The error must be reported as an exception
    // in the calling thread, instead of terminating the program (#91).
    std::string fmuPath(std::string(PROXYFMU_DATA_DIR) + "/fmus/2.0/20sim/ControlledTemperature.fmu");
    auto fmu = client::proxy_fmu(fmuPath);
    REQUIRE_THROWS_WITH(fmu.new_instance("instance"), Catch::Contains("No proxyfmu executable found"));
}
