
#include <fmi/fmu.hpp>

#include <iostream>

int main()
{
    std::string fmuPath("../fmus/2.0/cs/20sim/4.6.4.8004/ControlledTemperature/ControlledTemperature.fmu");

    auto fmu = fmi::loadFmu(fmuPath);

    auto md = fmu->get_model_description();
    std::cout << "GUID=" << md->get_guid() << std::endl;

    auto mv = md->get_model_variables();
    std::cout << mv.size() << std::endl;
//
//    auto slave = fmu->new_instance("instance");
//    slave->setup_experiment();
//    slave->enter_initialization_mode();
//    slave->exit_initialization_mode();
//    slave->terminate();
//    slave->freeInstance();

    return 0;
}