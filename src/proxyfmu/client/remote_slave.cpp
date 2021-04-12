
#include "remote_slave.hpp"

#include <proxyfmu/fs_portability.hpp>
#include <proxyfmu/thrift/BootService.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using namespace proxyfmu::thrift;

namespace
{

void start_process(const proxyfmu::filesystem::path& fmuPath, const std::string& instanceName, const int port)
{
    std::string cmd(
        "proxy_server"
        " --port " +
        std::to_string(port) +
        " --fmu \"" + fmuPath.string() + +"\""
                                          " --instanceName " +
        instanceName);
    auto status = system(cmd.c_str());
    std::cout << "External proxy process returned with status " << std::to_string(status) << std::endl;
}

void read_data(std::string const& fileName, std::string& data)
{
    FILE* file = fopen(fileName.c_str(), "rb");
    if (file == nullptr) return;
    fseek(file, 0, SEEK_END);
    long int size = ftell(file);
    fclose(file);
    file = fopen(fileName.c_str(), "rb");

    data.resize(size);
    size_t bytes_read = fread(data.data(), sizeof(unsigned char), size, file);
    fclose(file);
}

} // namespace


namespace proxyfmu::client
{

remote_slave::remote_slave(const filesystem::path& fmuPath, const std::string& instanceName, fmi::model_description modelDescription, const std::optional<remote_info>& remote)
    : rng_(49152, 65535)
    , modelDescription_(std::move(modelDescription))
{
    int port;
    std::string host;

    if (!remote) {
        port = rng_.next();
        host = "localhost";
        thread_ = std::make_unique<std::thread>(&start_process, fmuPath, instanceName, port);
    } else {
        host = remote->host;
        std::shared_ptr<TTransport> socket(new TSocket(host, remote->port));
        auto transport = std::make_shared<TFramedTransport>(socket);
        std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        auto client = std::make_shared<BootServiceClient>(protocol);
        transport->open();

        std::string data;
        read_data(fmuPath.string(), data);

        const std::string fmuName = proxyfmu::filesystem::path(fmuPath).stem().string();
        port = client->loadFromBinaryData(fmuName, instanceName, data);
        transport->close();
    }

    std::shared_ptr<TTransport> socket(new TSocket(host, port));
    transport_ = std::make_shared<TFramedTransport>(socket);
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport_));
    client_ = std::make_shared<FmuServiceClient>(protocol);
    transport_->open();
}

const fmi::model_description& remote_slave::get_model_description() const
{
    return modelDescription_;
}

bool remote_slave::setup_experiment(double start_time, double stop_time, double tolerance)
{
    auto status = client_->setup_experiment(start_time, stop_time, tolerance);
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool remote_slave::enter_initialization_mode()
{
    auto status = client_->enter_initialization_mode();
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool remote_slave::exit_initialization_mode()
{
    auto status = client_->exit_initialization_mode();
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool remote_slave::step(double current_time, double step_size)
{
    auto status = client_->step(current_time, step_size);
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool remote_slave::terminate()
{
    auto status = client_->terminate();
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool remote_slave::get_integer(const std::vector<fmi::value_ref>& vr, std::vector<int>& values)
{
    IntegerRead read;
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->read_integer(read, _vr);
    return read.status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool remote_slave::get_real(const std::vector<fmi::value_ref>& vr, std::vector<double>& values)
{
    assert(values.size() == vr.size());
    RealRead read;
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->read_real(read, _vr);
    for (auto i = 0; i < vr.size(); i++) {
        values[i] = read.value[i];
    }
    return read.status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool remote_slave::get_string(const std::vector<fmi::value_ref>& vr, std::vector<std::string>& values)
{
    assert(values.size() == vr.size());
    StringRead read;
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->read_string(read, _vr);
    for (auto i = 0; i < vr.size(); i++) {
        values[i] = read.value[i];
    }
    return read.status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool remote_slave::get_boolean(const std::vector<fmi::value_ref>& vr, std::vector<bool>& values)
{
    assert(values.size() == vr.size());
    BooleanRead read;
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->read_boolean(read, _vr);
    for (auto i = 0; i < vr.size(); i++) {
        values[i] = read.value[i];
    }
    return read.status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool remote_slave::set_integer(const std::vector<fmi::value_ref>& vr, const std::vector<int>& values)
{
    assert(values.size() == vr.size());
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    auto status = client_->write_integer(_vr, values);
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool remote_slave::set_real(const std::vector<fmi::value_ref>& vr, const std::vector<double>& values)
{
    assert(values.size() == vr.size());
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    auto status = client_->write_real(_vr, values);
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool remote_slave::set_string(const std::vector<fmi::value_ref>& vr, const std::vector<std::string>& values)
{
    assert(values.size() == vr.size());
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    auto status = client_->write_string(_vr, values);
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool remote_slave::set_boolean(const std::vector<fmi::value_ref>& vr, const std::vector<bool>& values)
{
    assert(values.size() == vr.size());
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    auto status = client_->write_boolean(_vr, values);
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

void remote_slave::freeInstance()
{
    if (!freed) {
        freed = true;
        client_->freeInstance();
        if (thread_) thread_->join();
    }
}

remote_slave::~remote_slave()
{
    remote_slave::freeInstance();
}

} // namespace proxyfmu::client
