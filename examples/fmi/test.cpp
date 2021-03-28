
#include <fmi/fmu.hpp>
#include "fmi/temp_dir.hpp"

#include <exception>
#include <iostream>

void run() {

    std::string fmuPath("../fmus/2.0/cs/20sim/4.6.4.8004/ControlledTemperature/ControlledTemperature.fmu");

    auto fmu = fmi::loadFmu(fmuPath);

    auto md = fmu->get_model_description();
    std::cout << "GUID=" << md.guid << std::endl;
    std::cout << "description=" << md.description << std::endl;
    std::cout << "generationTool=" << md.generation_tool << std::endl;

    auto mv = md.model_variables;
    std::cout << mv.size() << std::endl;
    std::cout << mv[0].name << std::endl;

    for (auto& v : mv) {
        auto attr = v.typeAttribute;
        std::visit([](fmi::type_attribute&& attr) {
                       if (std::holds_alternative<fmi::real>(attr)) {
                           auto start = std::get<fmi::real>(attr).start;
                           if (start) {
                               std::cout << "start=" << std::get<fmi::real>(attr).start.value() << std::endl;
                           }
                       }
                   },
                   attr);
    }

    auto slave = fmu->new_instance("instance");
    slave->setup_experiment();
    slave->enter_initialization_mode();
    slave->exit_initialization_mode();
    slave->step(0.0, 0.1);
    slave->terminate();
    slave->freeInstance();
}

int main()
{
    try {

        run();

    } catch (std::exception& ex) {
        std::cerr << "error: " << ex.what() << std::endl;
    }
    return 0;
}