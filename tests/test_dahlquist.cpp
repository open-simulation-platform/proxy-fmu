#include <proxyfmu/client/proxy_fmu.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

using namespace proxyfmu;
using namespace proxyfmu::fmi;

namespace
{

void test(fmu& fmu)
{
    const auto d = fmu.get_model_description();
    CHECK(d.modelName == "Dahlquist");
    CHECK(d.modelIdentifier == "Dahlquist");
    CHECK(d.guid == "{221063D2-EF4A-45FE-B954-B5BFEEA9A59B}");

    auto slave = fmu.new_instance("instance");
    REQUIRE(slave->setup_experiment());
    REQUIRE(slave->enter_initialization_mode());
    REQUIRE(slave->exit_initialization_mode());

    // The FMU solves x' = -k*x, with x(0) = 1 and k = 1, using forward Euler
    // with a fixed step size of 0.1.
    std::vector<value_ref> vr{1};
    std::vector<double> realRef(1);

    slave->get_real(vr, realRef);
    REQUIRE(realRef[0] == Approx(1.0));

    const double dt = 0.1;
    double t = 0;
    double expected = 1.0;
    for (int i = 0; i < 10; ++i) {
        REQUIRE(slave->step(t, dt));
        t += dt;
        expected *= 1.0 - dt;

        slave->get_real(vr, realRef);
        CHECK(realRef[0] == Approx(expected));
    }

    REQUIRE(slave->terminate());
    slave->freeInstance();
}

} // namespace

TEST_CASE("fmi_test_dahlquist")
{
    std::string fmuPath(std::string(REFERENCE_FMUS_DIR) + "/Dahlquist.fmu");
    auto fmu = loadFmu(fmuPath);
    test(*fmu);
}

TEST_CASE("client_test_dahlquist")
{
    std::string fmuPath(std::string(REFERENCE_FMUS_DIR) + "/Dahlquist.fmu");
    auto fmu = client::proxy_fmu(fmuPath);
    test(fmu);
}
