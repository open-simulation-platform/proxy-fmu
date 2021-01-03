
#include <fmuproxy/thrift/server/thrift_fmu_server.hpp>

#include <thrift/concurrency/ThreadManager.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TTransportUtils.h>

#include <iostream>
#include <utility>

using namespace fmuproxy::thrift;
using namespace fmuproxy::thrift::server;

using namespace ::apache::thrift;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

thrift_fmu_server::thrift_fmu_server(int port)
    : port_(port)
{
    std::shared_ptr<fmu_service_handler> handler(new fmu_service_handler());
    std::shared_ptr<TProcessor> processor(new FmuServiceProcessor(handler));

    std::shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
    std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    server_ = std::make_unique<TSimpleServer>(processor, serverTransport, transportFactory, protocolFactory);
}

void thrift_fmu_server::serve()
{
    server_->serve();
}

void thrift_fmu_server::start()
{
    std::cout << "FMU-proxy server listening to connections on port: " << std::to_string(port_) << std::endl;
    thread_ = std::make_unique<std::thread>(&thrift_fmu_server::serve, this);
}

void thrift_fmu_server::stop()
{
    server_->stop();
    thread_->join();
    std::cout << "FMU-proxy stopped.." << std::endl;
}
