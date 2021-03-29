#include <fmuproxy/thrift/server/fmu_service_handler.hpp>

using namespace fmuproxy::thrift;
using namespace fmuproxy::thrift::server;

#include <iostream>

namespace
{

void make_model_description(ModelDescription& _return, const fmuproxy::fmi::model_description& md)
{
    _return.guid = md.guid;
    _return.author = md.author;
    _return.model_name = md.model_name;
    _return.fmi_version = md.fmi_version;
    _return.description = md.description;
    _return.model_identifier = md.model_identifier;
    _return.generation_tool = md.generation_tool;
    _return.generation_date_and_time = md.generation_date_and_time;

    for (const auto& _v: md.model_variables) {
        ScalarVariable v;
        v.value_reference = _v.vr;
        v.name = _v.name;
        auto _attr = _v.typeAttribute;
        if (_v.is_real()) {
            RealAttribute r;
            auto _r = std::get<fmuproxy::fmi::real>(_attr);
            if (_r.start) {
                r.__set_start(*_r.start);
            }
            v.attribute.__set_real_attribute(r);
        } else if (_v.is_integer()) {
            IntegerAttribute i;
            auto _i = std::get<fmuproxy::fmi::integer>(_attr);
            if (_i.start) {
                i.__set_start(*_i.start);
            }
            v.attribute.__set_integer_attribute(i);
        } else if (_v.is_string()) {
            StringAttribute s;
            auto _s = std::get<fmuproxy::fmi::string>(_attr);
            if (_s.start) {
                s.__set_start(*_s.start);
            }
            v.attribute.__set_string_attribute(s);
        } else if (_v.is_boolean()) {
            BooleanAttribute b;
            auto _b = std::get<fmuproxy::fmi::boolean>(_attr);
            if (_b.start) {
                b.__set_start(*_b.start);
            }
            v.attribute.__set_boolean_attribute(b);
        }
        _return.model_variables.push_back(v);
    }
}

} // namespace

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
    return Status::ERROR_STATUS;
}

Status::type fmu_service_handler::enter_initialization_mode()
{
    slave_->enter_initialization_mode();
    return Status::ERROR_STATUS;
}

Status::type fmu_service_handler::exit_initialization_mode()
{
    slave_->exit_initialization_mode();
    return Status::ERROR_STATUS;
}
void fmu_service_handler::step(StepResult& _return, const double currentTime, const double stepSize)
{
    slave_->step(currentTime, stepSize);
}

Status::type fmu_service_handler::terminate()
{
    slave_->terminate();
    return Status::ERROR_STATUS;
}

void fmu_service_handler::freeInstance()
{
    slave_->freeInstance();
}


void fmu_service_handler::read_integer(IntegerRead& _return, const ValueReferences& vr)
{
}

void fmu_service_handler::read_real(RealRead& _return, const ValueReferences& vr)
{
}


void fmu_service_handler::read_string(StringRead& _return, const ValueReferences& vr)
{
}

void fmu_service_handler::read_boolean(BooleanRead& _return, const ValueReferences& vr)
{
}

Status::type fmu_service_handler::write_integer(const ValueReferences& vr, const IntArray& value)
{
    return Status::ERROR_STATUS;
}

Status::type fmu_service_handler::write_real(const ValueReferences& vr, const RealArray& value)
{
    return Status::ERROR_STATUS;
}

Status::type fmu_service_handler::write_string(const ValueReferences& vr, const StringArray& value)
{
    return Status::ERROR_STATUS;
}


Status::type fmu_service_handler::write_boolean(const ValueReferences& vr, const BooleanArray& value)
{
    return Status::ERROR_STATUS;
}
