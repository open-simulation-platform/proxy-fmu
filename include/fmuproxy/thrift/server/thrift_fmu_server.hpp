
#ifndef FMU_PROXY_THRIFT_THRIFTFMUSERVER_HPP
#define FMU_PROXY_THRIFT_THRIFTFMUSERVER_HPP

#include "fmu_service_handler.hpp"

#include <thrift/server/TServer.h>

#include <thread>
#include <unordered_map>

namespace fmuproxy::server
{

class thrift_fmu_server
{

private:
    const int port_;
    const std::string fmu_;
    std::unique_ptr<std::thread> thread_;
    std::unique_ptr<apache::thrift::server::TServer> server_;

    void serve();

public:
    thrift_fmu_server(const std::string& fmu, int port);

    void start();

    void stop();
};

} // namespace fmuproxy::thrift::server

#endif //FMU_PROXY_THRIFTSERVER_H
