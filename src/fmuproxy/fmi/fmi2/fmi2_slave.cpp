
#include "fmi2_slave.hpp"

#include <fmilib.h>

#include <exception>
#include <memory>


namespace fmuproxy::fmi
{

fmi2_slave::fmi2_slave(
    fmi2_import_t* fmu,
    const std::string& instanceName,
    model_description md,
    std::shared_ptr<fmuproxy::util::temp_dir> tmpDir)
    : fmu_(fmu)
    , md_(std::move(md))
    , tmpDir_(std::move(tmpDir))
{
    const auto rc = fmi2_import_instantiate(
        fmu_,
        instanceName.c_str(),
        fmi2_cosimulation,
        nullptr,
        fmi2_false);

    if (rc != jm_status_success) {
        fmi2_import_destroy_dllfmu(fmu_);
        fmi2_import_free(fmu_);
        throw std::runtime_error("failed to instantiate fmu!");
    }
}

const model_description& fmi2_slave::get_model_description() const
{
    return md_;
}

void fmi2_slave::setup_experiment(double start_time, double stop_time, double tolerance)
{
    fmi2_boolean_t tolerance_defined = tolerance > 0;
    fmi2_boolean_t stop_defined = tolerance > 0;
    fmi2_import_setup_experiment(fmu_, tolerance_defined, tolerance, start_time, stop_defined, stop_time);
}

void fmi2_slave::enter_initialization_mode()
{
    fmi2_import_enter_initialization_mode(fmu_);
}

void fmi2_slave::exit_initialization_mode()
{
    fmi2_import_exit_initialization_mode(fmu_);
}

void fmi2_slave::step(double current_time, double step_size)
{
    fmi2_import_do_step(fmu_, current_time, step_size, fmi2_true);
}

void fmi2_slave::terminate()
{
    fmi2_import_terminate(fmu_);
}


void fmi2_slave::freeInstance()
{
    if (!freed) {
        freed = true;
        fmi2_import_free_instance(fmu_);
    }
}

void fmi2_slave::get_integer(const std::vector<value_ref> &vr, std::vector<int> &values)
{
    fmi2_import_get_integer(fmu_, vr.data(), vr.size(), values.data());
}

void fmi2_slave::get_real(const std::vector<value_ref>& vr, std::vector<double>& values)
{
    fmi2_import_get_real(fmu_, vr.data(), vr.size(), values.data());
}

void fmi2_slave::get_string(const std::vector<value_ref>& vr, std::vector<std::string>& values)
{
    auto tmp = std::vector<fmi2_string_t>(vr.size());
    fmi2_import_get_string(fmu_, vr.data(), vr.size(), tmp.data());
    for (auto i = 0; i < tmp.size(); i++) {
        values[i] = tmp[i];
    }
}

void fmi2_slave::get_boolean(const std::vector<value_ref>& vr, std::vector<bool>& values)
{
    auto tmp = std::vector<fmi2_boolean_t>(vr.size());
    fmi2_import_get_boolean(fmu_, vr.data(), vr.size(), tmp.data());
    for (auto i = 0; i < tmp.size(); i++) {
        values[i] = tmp[i] != 0;
    }
}

void fmi2_slave::set_integer(const std::vector<value_ref>& vr, const std::vector<int>& values)
{
    fmi2_import_set_integer(fmu_, vr.data(), vr.size(), values.data());
}

void fmi2_slave::set_real(const std::vector<value_ref>& vr, const std::vector<double>& values)
{
    fmi2_import_set_real(fmu_, vr.data(), vr.size(), values.data());
}

void fmi2_slave::set_string(const std::vector<value_ref>& vr, const std::vector<std::string>& values)
{
    std::vector<fmi2_string_t> _values(vr.size());
    for (auto i = 0; i < vr.size(); i++) {
        _values[i] = values[i].c_str();
    }
    fmi2_import_set_string(fmu_, vr.data(), vr.size(), _values.data());
}

void fmi2_slave::set_boolean(const std::vector<value_ref>& vr, const std::vector<bool>& values)
{
    std::vector<fmi2_boolean_t> _values(vr.size());
    for (auto i = 0; i < vr.size(); i++) {
        _values[i] = values[i];
    }
    fmi2_import_set_boolean(fmu_, vr.data(), vr.size(), _values.data());
}

fmi2_slave::~fmi2_slave()
{
    fmi2_slave::freeInstance();
}

} // namespace fmi