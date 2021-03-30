
#include "remote_slave.hpp"

#include <utility>
#include <vector>

using namespace fmuproxy::thrift;

namespace fmuproxy::client
{

remote_slave::remote_slave(std::shared_ptr<FmuServiceClient> client, fmi::model_description modelDescription)
    : client_(std::move(client))
    , modelDescription_(std::move(modelDescription))
{
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
    StepResult result;
    client_->step(result, current_time, step_size);
}

void remote_slave::terminate()
{
    client_->terminate();
}

void remote_slave::freeInstance()
{
    client_->freeInstance();
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

remote_slave::~remote_slave()
{
    remote_slave::freeInstance();
}

} // namespace fmuproxy::client
