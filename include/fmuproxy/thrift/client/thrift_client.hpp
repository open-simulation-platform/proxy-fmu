
#ifndef FMU_PROXY_THRIFT_CLIENT_HPP
#define FMU_PROXY_THRIFT_CLIENT_HPP

#include <fmuproxy/thrift/FmuService.h>
#include <thrift/transport/TSocket.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TTransportUtils.h>

#include <fmi/model_description.hpp>

namespace fmuproxy::thrift::client
{

class thrift_client
{

private:
    std::shared_ptr<FmuServiceClient> client_;
    std::shared_ptr<apache::thrift::transport::TTransport> transport_;

    fmi::model_description modelDescription_;

public:

    thrift_client(const std::string &host, int port);

    [[nodiscard]] const fmi::model_description &get_model_description() const;

    void new_instance(const std::string &name);

    void close();
    ~thrift_client();

};

}


#endif //FMU_PROXY_THRIFT_CLIENT_HPP
