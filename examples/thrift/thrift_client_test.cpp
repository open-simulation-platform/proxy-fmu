
#include <iostream>
#include <exception>

#include <fmuproxy/thrift/client/thrift_client.hpp>

using namespace fmuproxy::client;

int main() {

    try {

        thrift_client client("localhost", 9090);

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

        auto slave = client.new_instance("instance");
        auto md2 = slave->get_model_description();
        std::cout << "GUID=" << md2.guid << std::endl;

        client.close();

    } catch (std::exception& tx) {
        std::cout << "ERROR: " << tx.what() << std::endl;
    }

}
