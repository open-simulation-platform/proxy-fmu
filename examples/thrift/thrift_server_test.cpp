
#include "../example_util.hpp"

#include <fmuproxy/thrift/server/thrift_fmu_server.hpp>

#include <memory>
#include <unordered_map>

using namespace fmuproxy::thrift::server;

int main() {

    auto fmu = "../fmus/2.0/cs/20sim/4.6.4.8004/ControlledTemperature/ControlledTemperature.fmu"

    thrift_fmu_server socket_server(fmu, 9090);
    socket_server.start();

    fmuproxy::wait_for_input();

    socket_server.stop();

    return 0;
}
