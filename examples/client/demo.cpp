
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
    slave->step(0, 0.1);
    slave->terminate();
    slave->freeInstance();
}

void run(proxy_fmu& fmu)
{
    auto slave = fmu.new_instance("instance1");
    run_slave(slave.get());
}

int main(int argc, char* argv[])
{

    if (argc != 2) return -1;

    try {

        proxyfmu::filesystem::path fmuLocation(argv[1]);
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

    } catch (std::exception& tx) {
        std::cout << "ERROR: " << tx.what() << std::endl;
    }
}
