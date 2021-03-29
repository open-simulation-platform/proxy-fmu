
#ifndef FMU_PROXY_THRIFT_CLIENT_HPP
#define FMU_PROXY_THRIFT_CLIENT_HPP

#include <fmuproxy/fmi/model_description.hpp>
#include <fmuproxy/thrift/FmuService.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

namespace fmuproxy::thrift::client
{

class thrift_client
{

private:
    std::shared_ptr<FmuServiceClient> client_;
    std::shared_ptr<apache::thrift::transport::TTransport> transport_;

    fmuproxy::fmi::model_description modelDescription_;

public:
    thrift_client(const std::string& host, int port);

    [[nodiscard]] const fmuproxy::fmi::model_description& get_model_description() const;

    void new_instance(const std::string& name);

    void close();
    ~thrift_client();
};

} // namespace fmuproxy::thrift::client


#endif //FMU_PROXY_THRIFT_CLIENT_HPP
