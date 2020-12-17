
#include "../../util/file_util.hpp"
#include "thrift_client_helper.hpp"

#include <fmuproxy/thrift/client/thrift_client.hpp>

#include <thrift/protocol/TCompactProtocol.h>

#include <utility>

using namespace std;
using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;
using namespace fmuproxy::thrift::client;

namespace fs = std::filesystem;

thrift_client::thrift_client(const string& host, const unsigned int port)
{
    shared_ptr<TTransport> socket(new TSocket(host, port));
    transport_ = std::make_shared<TFramedTransport>(socket);
    shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport_));
    client_ = std::make_shared<FmuServiceClient>(protocol);
    transport_->open();
}

remote_thrift_fmu thrift_client::from_file(const std::string& file)
{
    fs::path p(file);
    std::string name = p.stem().string();

    std::string data;
    read_data(file, data);

    FmuId guid;
    client_->load_from_file(guid, name, data);
    return remote_thrift_fmu(guid, client_);
}

remote_thrift_fmu thrift_client::from_guid(const std::string& guid)
{
    return remote_thrift_fmu(guid, client_);
}

void thrift_client::close()
{
    if (transport_->isOpen()) {
        transport_->close();
    }
}

thrift_client::~thrift_client()
{
    close();
}

remote_thrift_fmu::remote_thrift_fmu(FmuId fmuId, shared_ptr<FmuServiceClient> client)
    : fmuId_(std::move(fmuId))
    , client_(std::move(client))
{
}

shared_ptr<const fmi4cpp::fmi2::cs_model_description>& remote_thrift_fmu::getModelDescription()
{
    if (modelDescription_ == nullptr) {
        ModelDescription desc;
        client_->get_model_description(desc, fmuId_);
        modelDescription_ = convert(desc);
    }
    return modelDescription_;
}

unique_ptr<remote_fmu_slave> remote_thrift_fmu::newInstance()
{
    InstanceId instance_id;
    client_->create_instance(instance_id, fmuId_);
    return std::make_unique<remote_fmu_slave>(instance_id, *client_, getModelDescription());
}
