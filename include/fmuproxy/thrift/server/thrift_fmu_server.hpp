
#ifndef FMU_PROXY_THRIFTSERVER_H
#define FMU_PROXY_THRIFTSERVER_H

#include "../common/service_types.h"
#include "fmu_service_handler.hpp"

#include <fmi4cpp/fmi2/fmu.hpp>
#include <thrift/server/TServer.h>

#include <thread>
#include <unordered_map>


namespace fmuproxy::thrift::server
{

class thrift_fmu_server
{

private:
    const unsigned int port_;
    std::unique_ptr<std::thread> thread_;
    std::unique_ptr<apache::thrift::server::TServer> server_;

    void serve();

public:
    thrift_fmu_server(std::unordered_map<fmuproxy::thrift::FmuId,
                          std::shared_ptr<fmi4cpp::fmi2::cs_fmu>>& fmus,
        unsigned int port);

    void start();

    void stop();
};

} // namespace fmuproxy::thrift::server

#endif //FMU_PROXY_THRIFTSERVER_H
