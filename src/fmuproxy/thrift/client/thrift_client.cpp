
#include "fmuproxy/thrift/client/thrift_client.hpp"
#include <fmuproxy/thrift/FmuService.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "thrift_helper.hpp"
#include "remote_slave.hpp"

using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;

using namespace fmuproxy::fmi;
using namespace fmuproxy::thrift;
using namespace fmuproxy::client;

struct thrift_client::Impl
{

    model_description modelDescription_;
    std::shared_ptr<FmuServiceClient> client_;
    std::shared_ptr<apache::thrift::transport::TTransport> transport_;

    Impl(const std::string& host, int port)
    {
        std::shared_ptr<TTransport> socket(new TSocket(host, port));
        transport_ = std::make_shared<TFramedTransport>(socket);
        std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport_));
        client_ = std::make_shared<FmuServiceClient>(protocol);
        transport_->open();

        ModelDescription md;
        client_->get_model_description(md);
        fmuproxy::client::make_model_description(modelDescription_, md);
    }
};

thrift_client::thrift_client(const std::string& host, const int port)
    : pimpl_(std::make_unique<Impl>(host, port))
{
}

const model_description& thrift_client::get_model_description() const
{
    return pimpl_->modelDescription_;
}

std::unique_ptr<slave> thrift_client::new_instance(const std::string& name)
{
    pimpl_->client_->create_instance(name);
    return std::make_unique<remote_slave>(pimpl_->client_, pimpl_->modelDescription_);
}

void thrift_client::close()
{
    if (pimpl_->transport_->isOpen()) {
        pimpl_->transport_->close();
    }
}

thrift_client::~thrift_client()
{
    close();
}