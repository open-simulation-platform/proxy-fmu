
#include "fmi1_slave.hpp"

#include <fmilib.h>

#include <utility>


namespace
{
void fmilogger(fmi1_component_t c, fmi1_string_t instanceName, fmi1_status_t status, fmi1_string_t category, fmi1_string_t message, ...)
{
    va_list argp;
    va_start(argp, message);
    fmi1_log_forwarding_v(c, instanceName, status, category, message, argp);
    va_end(argp);
}
} // namespace


namespace proxyfmu::fmi
{

fmi1_slave::fmi1_slave(
    const std::shared_ptr<fmicontext>& ctx,
    const std::string& instanceName,
    model_description md,
    std::shared_ptr<temp_dir> tmpDir)
    : ctx_(ctx)
    , handle_(fmi1_import_parse_xml(ctx->ctx_, tmpDir->path().string().c_str()))
    , md_(std::move(md))
    , tmpDir_(std::move(tmpDir))
{

    fmi1_callback_functions_t callbackFunctions;
    callbackFunctions.allocateMemory = std::calloc;
    callbackFunctions.freeMemory = std::free;
    callbackFunctions.logger = fmilogger;

    if (fmi1_import_create_dllfmu(handle_, callbackFunctions, 1) != jm_status_success) {
        throw std::runtime_error(std::string("failed to load fmu dll! Error: ") + fmi1_import_get_last_error(handle_));
    }

    const auto rc = fmi1_import_instantiate_slave(
        handle_,
        instanceName.c_str(),
        nullptr,
        "",
        0,
        fmi1_false,
        fmi1_false);

    if (rc != jm_status_success) {
        fmi1_import_destroy_dllfmu(handle_);
        fmi1_import_free(handle_);
        throw std::runtime_error("failed to instantiate fmu!");
    }
}

const model_description& fmi1_slave::get_model_description() const
{
    return md_;
}

void fmi1_slave::setup_experiment(double start_time, double stop_time, double /*tolerance*/)
{
    start_time_ = start_time;
    stop_time_ = stop_time;
}

void fmi1_slave::enter_initialization_mode()
{
    //do nothing
}

void fmi1_slave::exit_initialization_mode()
{
    fmi1_boolean_t stop_defined = stop_time_ > 0;
    fmi1_import_initialize_slave(handle_, start_time_, stop_defined, stop_time_);
}

void fmi1_slave::step(double current_time, double step_size)
{
    fmi1_import_do_step(handle_, current_time, step_size, fmi1_true);
}

void fmi1_slave::terminate()
{
    fmi1_import_terminate(handle_);
}

void fmi1_slave::freeInstance()
{
    if (!freed) {
        freed = true;
        fmi1_import_free_slave_instance(handle_);
        fmi1_import_destroy_dllfmu(handle_);
        fmi1_import_free(handle_);
    }
}


void fmi1_slave::get_integer(const std::vector<value_ref>& vr, std::vector<int>& values)
{
    fmi1_import_get_integer(handle_, vr.data(), vr.size(), values.data());
}

void fmi1_slave::get_real(const std::vector<value_ref>& vr, std::vector<double>& values)
{
    fmi1_import_get_real(handle_, vr.data(), vr.size(), values.data());
}

void fmi1_slave::get_string(const std::vector<value_ref>& vr, std::vector<std::string>& values)
{
    auto tmp = std::vector<fmi1_string_t>(vr.size());
    fmi1_import_get_string(handle_, vr.data(), vr.size(), tmp.data());
    for (auto i = 0; i < tmp.size(); i++) {
        values[i] = tmp[i];
    }
}

void fmi1_slave::get_boolean(const std::vector<value_ref>& vr, std::vector<bool>& values)
{
    auto tmp = std::vector<fmi1_boolean_t>(vr.size());
    fmi1_import_get_boolean(handle_, vr.data(), vr.size(), tmp.data());
    for (auto i = 0; i < tmp.size(); i++) {
        values[i] = tmp[i] != 0;
    }
}

void fmi1_slave::set_integer(const std::vector<value_ref>& vr, const std::vector<int>& values)
{
    fmi1_import_set_integer(handle_, vr.data(), vr.size(), values.data());
}

void fmi1_slave::set_real(const std::vector<value_ref>& vr, const std::vector<double>& values)
{
    fmi1_import_set_real(handle_, vr.data(), vr.size(), values.data());
}

void fmi1_slave::set_string(const std::vector<value_ref>& vr, const std::vector<std::string>& values)
{
    std::vector<fmi1_string_t> _values(vr.size());
    for (auto i = 0; i < vr.size(); i++) {
        _values[i] = values[i].c_str();
    }
    fmi1_import_set_string(handle_, vr.data(), vr.size(), _values.data());
}

void fmi1_slave::set_boolean(const std::vector<value_ref>& vr, const std::vector<bool>& values)
{
    std::vector<fmi1_boolean_t> _values(vr.size());
    for (auto i = 0; i < vr.size(); i++) {
        _values[i] = values[i];
    }
    fmi1_import_set_boolean(handle_, vr.data(), vr.size(), _values.data());
}

fmi1_slave::~fmi1_slave()
{
    fmi1_slave::freeInstance();
}

} // namespace proxyfmu::fmi