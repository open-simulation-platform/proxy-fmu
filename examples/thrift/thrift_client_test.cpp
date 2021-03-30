
#include <fmuproxy/thrift/client/proxy_client.hpp>

#include <exception>
#include <iostream>

using namespace fmuproxy;
using namespace fmuproxy::client;

int main() {

    try {

        proxy_client client("localhost", 9090);

        auto md = client.get_model_description();
        std::cout << "GUID=" << md.guid << std::endl;

        auto mv = md.model_variables;
        std::cout << mv.size() << std::endl;

        for (const auto& v : mv) {
            auto attr = v.typeAttribute;
            if (v.is_real()) {
                auto start = std::get<fmuproxy::fmi::real>(attr).start;
                if (start) {
                    std::cout << "start=" << std::get<fmuproxy::fmi::real>(attr).start.value() << std::endl;
                }
            }
        }

        {
            auto slave = client.new_instance("instance");
            auto md2 = slave->get_model_description();
            std::cout << "GUID=" << md2.guid << std::endl;
            slave->setup_experiment();
            slave->enter_initialization_mode();
            slave->exit_initialization_mode();
            std::vector<fmi::value_ref> vr{47};
            std::vector<double> ref(1);
            slave->get_real(vr, ref);
            std::cout << "value=" << ref[0] << std::endl;
        }

        client.close();

    } catch (std::exception& tx) {
        std::cout << "ERROR: " << tx.what() << std::endl;
    }

}
