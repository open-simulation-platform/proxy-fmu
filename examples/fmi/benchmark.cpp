
#include <vector>
#include <iostream>

#include "../example_util.hpp"
#include <fmi4cpp/fmi4cpp.hpp>

using namespace std;
using namespace fmi4cpp;

const double start = 0.0;
const double stop = 10.0;
const double step_size = 1E-4;

int main() {

    const string fmu_path = "../fmus/2.0/cs/20sim/4.6.4.8004/"
                            "ControlledTemperature/ControlledTemperature.fmu";

    auto fmu = fmi2::fmu(fmu_path).as_cs_fmu();
    auto slave = fmu->new_instance();
    auto md = slave->get_model_description();
    slave->setup_experiment();
    slave->enter_initialization_mode();
    slave->exit_initialization_mode();

    vector<fmi2Real > ref(2);
    vector<fmi2ValueReference > vr = {md->get_value_reference("Temperature_Reference"),
                                      md->get_value_reference("Temperature_Room")};

    auto elapsed = fmuproxy::measure_time_sec([&slave, &vr, &ref]{

        while ( (slave->get_simulation_time() ) <= (stop-step_size) ) {
            if (!slave->step(step_size)) {
                cout << "Error! doStep returned with status: " << to_string(slave->last_status()) << endl;
                break;
            }
            slave->read_real(vr, ref);
        }
    });

    cout << "elapsed=" << elapsed << "s" << endl;

    slave->terminate();

    return 0;

}