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
    CHECK(d.modelName == "no.viproma.demo.identity");
    CHECK(d.description ==
        "Has one input and one output of each type, and outputs are always set equal to inputs");
    CHECK(d.author == "Lars Tandle Kyllingstad");

    auto slave = fmu.new_instance("instance");
    REQUIRE(slave->setup_experiment());
    REQUIRE(slave->enter_initialization_mode());
    REQUIRE(slave->exit_initialization_mode());

    std::vector<value_ref> vr{0};

    std::vector<double> realVal{0.0};
    std::vector<int> integerVal{0};
    std::vector<bool> booleanVal{false};
    std::vector<std::string> stringVal{""};

    std::vector<double> realRef(1);
    std::vector<int> integerRef(1);
    std::vector<bool> booleanRef(1);
    std::vector<std::string> stringRef(1);

    double t = 0.0;
    double tEnd = 1.0;
    double dt = 0.1;

    while (t <= tEnd) {

        slave->get_real(vr, realRef);
        slave->get_integer(vr, integerRef);
        slave->get_boolean(vr, booleanRef);
        slave->get_string(vr, stringRef);

        CHECK(realVal[0] == realRef[0]);
        CHECK(integerVal[0] == integerRef[0]);
        CHECK(booleanVal[0] == booleanRef[0]);
        CHECK(stringVal[0] == stringRef[0]);

        REQUIRE(slave->step(t, dt));

        realVal[0] += 1.0;
        integerVal[0] += 1;
        booleanVal[0] = !booleanVal[0];
        stringVal[0] += 'a';

        slave->set_real(vr, realVal);
        slave->set_integer(vr, integerVal);
        slave->set_boolean(vr, booleanVal);
        slave->set_string(vr, stringVal);

        t += dt;
    }

    REQUIRE(slave->terminate());
    slave->freeInstance();
}

void test_batched(fmu& fmu)
{
    auto slave = fmu.new_instance("instance");
    REQUIRE(slave->setup_experiment());
    REQUIRE(slave->enter_initialization_mode());
    REQUIRE(slave->exit_initialization_mode());

    const variable_refs vr{{0}, {0}, {0}, {0}};
    variable_values values{{0}, {0.0}, {""}, {false}};
    variable_values read;

    REQUIRE(slave->get_all(vr, read));
    CHECK(read.integers == values.integers);
    CHECK(read.reals == values.reals);
    CHECK(read.strings == values.strings);
    CHECK(read.booleans == values.booleans);

    double t = 0.0;
    double tEnd = 1.0;
    double dt = 0.1;

    while (t <= tEnd) {
        values.integers[0] += 1;
        values.reals[0] += 1.0;
        values.strings[0] += 'a';
        values.booleans[0] = !values.booleans[0];

        REQUIRE(slave->step_exchange(vr, values, t, dt, vr, read));
        CHECK(read.integers == values.integers);
        CHECK(read.reals == values.reals);
        CHECK(read.strings == values.strings);
        CHECK(read.booleans == values.booleans);

        t += dt;
    }

    values.reals[0] = -1.0;
    REQUIRE(slave->set_all(vr, values));
    REQUIRE(slave->get_all(vr, read));
    CHECK(read.reals == values.reals);

    // Nothing to set and nothing to get.
    REQUIRE(slave->step_exchange({}, {}, t, dt, {}, read));
    CHECK(read.reals.empty());
    CHECK(read.strings.empty());

    REQUIRE(slave->terminate());
    slave->freeInstance();
}

} // namespace

TEST_CASE("fmi_test_identity")
{
    std::string fmuPath(std::string(PROXYFMU_DATA_DIR) + "/fmus/1.0/identity.fmu");
    auto fmu = loadFmu(fmuPath);
    test(*fmu);
}

TEST_CASE("client_test_identity")
{
    std::string fmuPath(std::string(PROXYFMU_DATA_DIR) + "/fmus/1.0/identity.fmu");
    auto fmu = client::proxy_fmu(fmuPath);
    test(fmu);
}
TEST_CASE("fmi_test_identity_batched")
{
    std::string fmuPath(std::string(PROXYFMU_DATA_DIR) + "/fmus/1.0/identity.fmu");
    auto fmu = loadFmu(fmuPath);
    test_batched(*fmu);
}

TEST_CASE("client_test_identity_batched")
{
    std::string fmuPath(std::string(PROXYFMU_DATA_DIR) + "/fmus/1.0/identity.fmu");
    auto fmu = client::proxy_fmu(fmuPath);
    test_batched(fmu);
}
