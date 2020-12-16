
#include <ctime>
#include <iostream>

#include <fmuproxy/thrift/common/fmu_service.h>
#include <fmuproxy/thrift/client/thrift_client.hpp>

#include "../example_util.hpp"

using namespace apache::thrift;

using namespace fmuproxy::thrift;
using namespace fmuproxy::thrift::client;

int main() {

    try {

        thrift_client client("localhost", 9090);

        auto fmu = client.from_guid("{06c2700b-b39c-4895-9151-304ddde28443}");
        fmuproxy::run_slave(fmu.newInstance());

        auto remote_fmu = client.from_file("../fmus/2.0/cs/20sim/4.6.4.8004/"
                                           "ControlledTemperature/ControlledTemperature.fmu");
        fmuproxy::run_slave(remote_fmu.newInstance());

        client.close();

    } catch (TException& tx) {
        std::cout << "ERROR: " << tx.what() << std::endl;
    }

}
