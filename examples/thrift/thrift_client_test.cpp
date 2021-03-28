
#include <iostream>

#include <fmuproxy/thrift/client/thrift_client.hpp>

using namespace apache::thrift;

using namespace fmuproxy::thrift;
using namespace fmuproxy::thrift::client;

int main() {

    try {

        thrift_client client("localhost", 9090);

        auto md = client.get_model_description();
        std::cout << "GUID=" << md.guid << std::endl;

        client.close();

    } catch (TException& tx) {
        std::cout << "ERROR: " << tx.what() << std::endl;
    }

}
