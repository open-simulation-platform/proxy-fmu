#define BOOST_TEST_MODULE test_controlled_temp

#include <proxyfmu/client/proxy_fmu.hpp>

#include <boost/test/unit_test.hpp>

using namespace proxyfmu;
using namespace proxyfmu::fmi;

namespace
{

void test(fmu& fmu)
{
    const auto d = fmu.get_model_description();
    BOOST_TEST(d.modelName == "ControlledTemperature");
    BOOST_TEST(d.modelIdentifier == "ControlledTemperature");
    BOOST_TEST(d.guid == "{06c2700b-b39c-4895-9151-304ddde28443}");
    BOOST_TEST(d.generationTool == "20-sim");

    try {
        auto slave = fmu.new_instance("instance");
        BOOST_REQUIRE(slave->setup_experiment());
        BOOST_REQUIRE(slave->enter_initialization_mode());
        BOOST_REQUIRE(slave->exit_initialization_mode());

        std::vector<value_ref> vr{47};
        std::vector<double> realRef(1);

        slave->get_real(vr, realRef);
        BOOST_REQUIRE_CLOSE(realRef[0], 298, 0.0001);

        BOOST_REQUIRE(slave->step(0.0, 0.1));

        slave->get_real(vr, realRef);
        BOOST_TEST(realRef[0] < 298);

        BOOST_REQUIRE(slave->terminate());
        slave->freeInstance();
    } catch (std::runtime_error &ex) {
        BOOST_FAIL(std::string("Runtime error encountered: ") + ex.what());
    }
}

} // namespace

BOOST_AUTO_TEST_CASE(fmi_test_controlled_temp)
{
    std::string fmuPath("../fmus/2.0/20sim/ControlledTemperature.fmu");
    auto fmu = loadFmu(fmuPath);
    test(*fmu);
}

BOOST_AUTO_TEST_CASE(client_test_controlled_temp)
{
    std::string fmuPath("../fmus/2.0/20sim/ControlledTemperature.fmu");
    auto fmu = client::proxy_fmu(fmuPath);
    test(fmu);
}