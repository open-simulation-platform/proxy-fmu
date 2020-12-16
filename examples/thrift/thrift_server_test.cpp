
#include "../example_util.hpp"

#include <fmuproxy/thrift/server/thrift_fmu_server.hpp>

#include <memory>
#include <unordered_map>

using namespace fmi4cpp;
using namespace fmuproxy::thrift::server;

int main() {

    auto fmu1 = fmi2::fmu("../fmus/2.0/cs/20sim/4.6.4.8004/ControlledTemperature/ControlledTemperature.fmu").as_cs_fmu();

    std::unordered_map<std::string, std::shared_ptr<fmi2::cs_fmu>> fmus = {
            {fmu1->get_model_description()->guid, std::move(fmu1)}
    };

    thrift_fmu_server socket_server(fmus, 9090);
    socket_server.start();

    fmuproxy::wait_for_input();

    socket_server.stop();

    return 0;
}
