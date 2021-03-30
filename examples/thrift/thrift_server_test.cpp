
#include <cstdlib>

int main()
{
    system(
        "fmu_proxy "
        "--port 9090 "
        "--fmu "
        "../fmus/2.0/cs/20sim/4.6.4.8004/ControlledTemperature/ControlledTemperature.fmu");

    return 0;
}
