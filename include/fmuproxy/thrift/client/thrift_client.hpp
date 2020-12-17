
#ifndef FMU_PROXY_THRIFT_THRIFTCLIENT_HPP
#define FMU_PROXY_THRIFT_THRIFTCLIENT_HPP

#include <thrift/transport/TSocket.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TTransportUtils.h>

#include "remote_fmu_slave.hpp"

namespace fmuproxy::thrift::client {

    class remote_thrift_fmu {

    private:

        const FmuId fmuId_;
        std::shared_ptr<FmuServiceClient> client_;
        std::shared_ptr<const fmi4cpp::fmi2::cs_model_description> modelDescription_;

    public:

        remote_thrift_fmu(FmuId fmuId, std::shared_ptr<FmuServiceClient> client);

        std::shared_ptr<const fmi4cpp::fmi2::cs_model_description> &getModelDescription();

        std::unique_ptr<remote_fmu_slave> newInstance();

    };

    class thrift_client {

    private:

        std::shared_ptr<FmuServiceClient> client_;
        std::shared_ptr<apache::thrift::transport::TTransport> transport_;

    public:

        thrift_client(const std::string &host, unsigned int port);

        remote_thrift_fmu from_file(const std::string &file);

        remote_thrift_fmu from_guid(const std::string &guid);

        void close();

        virtual ~thrift_client();

    };

}

#endif //FMU_PROXY_THRIFTCLIENT_H
