#include <proxyfmu/client/proxy_fmu.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

using namespace proxyfmu;

TEST_CASE("client_test_load_failure")
{
    // The proxy process starts, but can't load the FMU's binary. This must be
    // reported as an exception, and the proxy process must shut down (if it
    // doesn't, this test hangs until CTest times out).
    auto fmu = client::proxy_fmu(NO_BINARIES_FMU);
    REQUIRE_THROWS(fmu.new_instance("instance"));
}
