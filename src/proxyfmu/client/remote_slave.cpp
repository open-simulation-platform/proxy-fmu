
#include "remote_slave.hpp"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <iostream>
#include <utility>
#include <vector>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using namespace proxyfmu::thrift;

namespace
{

void start_process(const std::filesystem::path& fmu, const std::string& instanceName, const int port)
{
    std::string cmd(
        "proxy_server"
        " --port " +
        std::to_string(port) +
        " --fmu \"" + fmu.string() + +"\""
                                      " --instanceName " +
        instanceName);
    auto status = system(cmd.c_str());
    std::cout << "External proxy process returned with status " << std::to_string(status) << std::endl;
}

} // namespace


namespace proxyfmu::client
{

remote_slave::remote_slave(const std::filesystem::path& fmu, const std::string& instanceName, fmi::model_description modelDescription)
    : modelDescription_(std::move(modelDescription))
{

    const int port = 9090;
    thread_ = std::make_unique<std::thread>(&start_process, fmu, instanceName, port);

    std::shared_ptr<TTransport> socket(new TSocket("localhost", port));
    transport_ = std::make_shared<TFramedTransport>(socket);
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport_));
    client_ = std::make_shared<FmuServiceClient>(protocol);
    transport_->open();
}

void remote_slave::setup_experiment(double start_time, double stop_time, double tolerance)
{
    client_->setup_experiment(start_time, stop_time, tolerance);
}

const fmi::model_description& remote_slave::get_model_description() const
{
    return modelDescription_;
}

void remote_slave::enter_initialization_mode()
{
    client_->enter_initialization_mode();
}

void remote_slave::exit_initialization_mode()
{
    client_->exit_initialization_mode();
}

void remote_slave::step(double current_time, double step_size)
{
    client_->step(current_time, step_size);
}

void remote_slave::terminate()
{
    client_->terminate();
}

void remote_slave::get_integer(const std::vector<fmi::value_ref>& vr, std::vector<int>& values)
{
    IntegerRead read;
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->read_integer(read, _vr);
}

void remote_slave::get_real(const std::vector<fmi::value_ref>& vr, std::vector<double>& values)
{
    assert(values.size() == vr.size());
    RealRead read;
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->read_real(read, _vr);
    for (auto i = 0; i < vr.size(); i++) {
        values[i] = read.value[i];
    }
}

void remote_slave::get_string(const std::vector<fmi::value_ref>& vr, std::vector<std::string>& values)
{
    assert(values.size() == vr.size());
    StringRead read;
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->read_string(read, _vr);
    for (auto i = 0; i < vr.size(); i++) {
        values[i] = read.value[i];
    }
}

void remote_slave::get_boolean(const std::vector<fmi::value_ref>& vr, std::vector<bool>& values)
{
    assert(values.size() == vr.size());
    BooleanRead read;
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->read_boolean(read, _vr);
    for (auto i = 0; i < vr.size(); i++) {
        values[i] = read.value[i];
    }
}

void remote_slave::set_integer(const std::vector<fmi::value_ref>& vr, const std::vector<int>& values)
{
    assert(values.size() == vr.size());
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->write_integer(_vr, values);
}

void remote_slave::set_real(const std::vector<fmi::value_ref>& vr, const std::vector<double>& values)
{
    assert(values.size() == vr.size());
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->write_real(_vr, values);
}

void remote_slave::set_string(const std::vector<fmi::value_ref>& vr, const std::vector<std::string>& values)
{
    assert(values.size() == vr.size());
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->write_string(_vr, values);
}

void remote_slave::set_boolean(const std::vector<fmi::value_ref>& vr, const std::vector<bool>& values)
{
    assert(values.size() == vr.size());
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->write_boolean(_vr, values);
}

void remote_slave::freeInstance()
{
    if (!freed) {
        freed = true;
        client_->freeInstance();
        thread_->join();
    }
}

remote_slave::~remote_slave()
{
    remote_slave::freeInstance();
}

} // namespace proxyfmu::client
