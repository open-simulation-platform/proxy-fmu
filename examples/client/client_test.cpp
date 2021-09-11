
#include <proxyfmu/client/proxy_fmu.hpp>

#include <exception>
#include <iostream>

using namespace proxyfmu;
using namespace proxyfmu::client;

void run_slave(fmi::slave* slave)
{
    auto md2 = slave->get_model_description();
    std::cout << "GUID=" << md2.guid << std::endl;
    slave->setup_experiment();
    slave->enter_initialization_mode();
    slave->exit_initialization_mode();
    std::vector<fmi::value_ref> vr{47};
    std::vector<double> ref(1);
    slave->get_real(vr, ref);
    std::cout << "value=" << ref[0] << std::endl;
    slave->terminate();
    slave->freeInstance();
}

void run(proxy_fmu& fmu) {
    {
        auto slave = fmu.new_instance("instance1");
        run_slave(slave.get());

    }

    {
        auto slave = fmu.new_instance("instance2");
        run_slave(slave.get());
    }
}

int main() {

    try {

        proxyfmu::filesystem::path fmuLocation("../fmus/2.0/20sim/ControlledTemperature.fmu");

        proxy_fmu fmu(fmuLocation);

        auto md = fmu.get_model_description();
        std::cout << "GUID=" << md.guid << std::endl;

        auto mv = md.modelVariables;
        std::cout << mv.size() << std::endl;

        for (const auto& v : mv) {
            auto attr = v.typeAttribute;
            if (v.is_real()) {
                auto start = std::get<proxyfmu::fmi::real>(attr).start;
                if (start) {
                    std::cout << "start=" << std::get<proxyfmu::fmi::real>(attr).start.value() << std::endl;
                }
            }
        }

        run(fmu);

//        proxy_fmu remoteFmu(fmuLocation, remote_info("localhost", 9090));
//        run(remoteFmu);

    } catch (std::exception& tx) {
        std::cout << "ERROR: " << tx.what() << std::endl;
    }

}
