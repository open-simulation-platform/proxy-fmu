
#include <cstdlib>
#include <string>

int main()
{
    std::string cmd(
        "fmu_proxy "
        "--port " + std::to_string(9090) + " " +
        "--fmu ../fmus/2.0/cs/20sim/4.6.4.8004/ControlledTemperature/ControlledTemperature.fmu"
    );
    system(cmd.c_str());

    return 0;
}
