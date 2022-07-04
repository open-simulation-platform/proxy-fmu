
#include <proxyfmu/client/proxy_fmu.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

using namespace proxyfmu;

namespace
{

void test(fmilibcpp::fmu& fmu)
{
    const auto d = fmu.get_model_description();
    CHECK(d.modelName == "ControlledTemperature");
    CHECK(d.modelIdentifier == "ControlledTemperature");
    CHECK(d.guid == "{06c2700b-b39c-4895-9151-304ddde28443}");
    CHECK(d.generationTool == "20-sim");

    auto slave = fmu.new_instance("instance");
    REQUIRE(slave->setup_experiment());
    REQUIRE(slave->enter_initialization_mode());
    REQUIRE(slave->exit_initialization_mode());

    std::vector<fmilibcpp::value_ref> vr{47};
    std::vector<double> realRef(1);

    slave->get_real(vr, realRef);
    REQUIRE(realRef[0] == Approx(298));

    REQUIRE(slave->step(0.0, 0.1));

    slave->get_real(vr, realRef);
    CHECK(realRef[0] < 298);

    REQUIRE(slave->terminate());
    slave->freeInstance();
}

} // namespace

TEST_CASE("fmi_test_controlled_temp")
{
    std::string fmuPath("../fmus/2.0/20sim/ControlledTemperature.fmu");
    auto fmu = fmilibcpp::loadFmu(fmuPath);
    test(*fmu);
}

TEST_CASE("client_test_controlled_temp")
{
    std::string fmuPath("../fmus/2.0/20sim/ControlledTemperature.fmu");
    auto fmu = client::proxy_fmu(fmuPath);
    test(fmu);
}