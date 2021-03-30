#include <fmuproxy/thrift/server/fmu_service_handler.hpp>

using namespace fmuproxy::thrift;
using namespace fmuproxy::server;

#include "thrift_helper.hpp"

#include <iostream>

fmu_service_handler::fmu_service_handler(const std::string& fmu)
{
    fmu_ = fmi::loadFmu(fmu);
}

void fmu_service_handler::get_model_description(ModelDescription& _return)
{
    auto md = fmu_->get_model_description();
    make_model_description(_return, md);
}

void fmu_service_handler::create_instance(const std::string& instanceName)
{
    slave_ = fmu_->new_instance(instanceName);
}

Status::type fmu_service_handler::setup_experiment(const double start, const double stop, const double tolerance)
{
    slave_->setup_experiment(start, stop, tolerance);
    return Status::OK_STATUS;
}

Status::type fmu_service_handler::enter_initialization_mode()
{
    slave_->enter_initialization_mode();
    return Status::OK_STATUS;
}

Status::type fmu_service_handler::exit_initialization_mode()
{
    slave_->exit_initialization_mode();
    return Status::OK_STATUS;
}
void fmu_service_handler::step(StepResult& _return, const double currentTime, const double stepSize)
{
    slave_->step(currentTime, stepSize);
}

Status::type fmu_service_handler::terminate()
{
    slave_->terminate();
    return Status::OK_STATUS;
}

void fmu_service_handler::freeInstance()
{
    slave_->freeInstance();
}


void fmu_service_handler::read_integer(IntegerRead& _return, const ValueReferences& vr)
{
    const std::vector<fmi::value_ref> _vr(vr.begin(), vr.end());
    _return.value.resize(vr.size());
    slave_->get_integer(_vr, _return.value);
}

void fmu_service_handler::read_real(RealRead& _return, const ValueReferences& vr)
{
    const std::vector<fmi::value_ref> _vr(vr.begin(), vr.end());
    _return.value.resize(vr.size());
    slave_->get_real(_vr, _return.value);
}


void fmu_service_handler::read_string(StringRead& _return, const ValueReferences& vr)
{
    const std::vector<fmi::value_ref> _vr(vr.begin(), vr.end());
    _return.value.resize(vr.size());
    slave_->get_string(_vr, _return.value);
}

void fmu_service_handler::read_boolean(BooleanRead& _return, const ValueReferences& vr)
{
    const std::vector<fmi::value_ref> _vr(vr.begin(), vr.end());
    _return.value.resize(vr.size());
    slave_->get_boolean(_vr, _return.value);
}

Status::type fmu_service_handler::write_integer(const ValueReferences& vr, const IntArray& value)
{
    const std::vector<fmi::value_ref> _vr(vr.begin(), vr.end());
    slave_->set_integer(_vr, value);
    return Status::OK_STATUS;
}

Status::type fmu_service_handler::write_real(const ValueReferences& vr, const RealArray& value)
{
    const std::vector<fmi::value_ref> _vr(vr.begin(), vr.end());
    slave_->set_real(_vr, value);
    return Status::OK_STATUS;
}

Status::type fmu_service_handler::write_string(const ValueReferences& vr, const StringArray& value)
{
    const std::vector<fmi::value_ref> _vr(vr.begin(), vr.end());
    slave_->set_string(_vr, value);
    return Status::OK_STATUS;
}


Status::type fmu_service_handler::write_boolean(const ValueReferences& vr, const BooleanArray& value)
{
    const std::vector<fmi::value_ref> _vr(vr.begin(), vr.end());
    slave_->set_boolean(_vr, value);
    return Status::OK_STATUS;
}
