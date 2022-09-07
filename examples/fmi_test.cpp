
#include <proxyfmu/fmi/fmu.hpp>

#include <CLI/CLI.hpp>

#include <exception>
#include <iostream>

using namespace proxyfmu::fmi;

void run(const std::string& fmuPath)
{

    auto fmu = loadFmu(fmuPath);

    auto md = fmu->get_model_description();
    std::cout << "GUID=" << md.guid << std::endl;
    std::cout << "description=" << md.description << std::endl;
    std::cout << "generationTool=" << md.generationTool << std::endl;

    auto mv = md.modelVariables;
    std::cout << "modelVariables size=" << mv.size() << std::endl;

    std::vector<std::string> real_names;
    std::vector<value_ref> real_refs;
    for (auto& v : mv) {
        auto attr = v.typeAttribute;
        if (v.is_real() && v.causality == "output") {
            real_names.push_back(v.name);
            real_refs.push_back(v.vr);
            auto start = std::get<real>(attr).start;
            if (start) {
                std::cout << v.name << " start=" << std::get<real>(attr).start.value() << std::endl;
            }
        }
    }

    auto slave = fmu->new_instance("instance");

    slave->setup_experiment();
    slave->enter_initialization_mode();
    slave->exit_initialization_mode();

    double t = 0;
    double dt = 0.1;
    std::vector<double> values(real_refs.size());
    while (t < 1) {
        slave->step(t, dt);
        slave->get_real(real_refs, values);
        for (unsigned i = 0; i < real_refs.size(); i++) {
            std::cout << "t=" << t << ", " << real_names[i] << "=" << values[i] << "\n";
        }
        t += dt;
    }

    slave->terminate();
    slave->freeInstance();
}

int main(int argc, char** argv)
{

    CLI::App app{"fmi_test"};

    std::string fmuPath = std::string(PROXYFMU_DATA_DIR) + "/fmus/1.0/identity.fmu";
    app.add_option("--fmu", fmuPath);

    CLI11_PARSE(app, argc, argv);

    try {

        run(fmuPath);

    } catch (std::exception& ex) {
        std::cerr << "error: " << ex.what() << std::endl;
    }
    return 0;
}
