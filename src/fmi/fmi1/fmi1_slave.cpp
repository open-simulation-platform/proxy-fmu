
#include "fmi1_slave.hpp"
#include <fmilib.h>

#include <utility>

namespace fmi
{

fmi1_slave::fmi1_slave(
    fmi1_import_t* fmu,
    const std::string& instanceName,
    model_description md,
    std::shared_ptr<temp_dir> tmpDir)
    : fmu_(fmu)
    , md_(std::move(md))
    , tmpDir_(std::move(tmpDir))
{
    const auto rc = fmi1_import_instantiate_slave(
        fmu,
        instanceName.c_str(),
        nullptr,
        "",
        0,
        fmi1_false,
        fmi1_false);

    if (rc != jm_status_success) {
        fmi1_import_destroy_dllfmu(fmu);
        fmi1_import_free(fmu);
        throw std::runtime_error("failed to instantiate fmu!");
    }
}

model_description fmi1_slave::get_model_description() const
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
    fmi1_import_initialize_slave(fmu_, start_time_, stop_defined, stop_time_);
}

void fmi1_slave::step(double current_time, double step_size)
{
    fmi1_import_do_step(fmu_, current_time, step_size, fmi1_true);
}

void fmi1_slave::terminate()
{
    fmi1_import_terminate(fmu_);
}

void fmi1_slave::freeInstance()
{
    if (!freed) {
        freed = true;
        fmi1_import_free_slave_instance(fmu_);
        fmi1_import_destroy_dllfmu(fmu_);
        fmi1_import_free(fmu_);
    }
}


void fmi1_slave::get_integer(const std::vector<value_ref> &vr, std::vector<int> &values)
{
    fmi1_import_get_integer(fmu_, vr.data(), vr.size(), values.data());
}

void fmi1_slave::get_real(const std::vector<value_ref>& vr, std::vector<double>& values)
{
    fmi1_import_get_real(fmu_, vr.data(), vr.size(), values.data());
}

void fmi1_slave::get_string(const std::vector<value_ref>& vr, std::vector<std::string>& values)
{
    auto tmp = std::vector<fmi1_string_t>(vr.size());
    fmi1_import_get_string(fmu_, vr.data(), vr.size(), tmp.data());
    for (auto i = 0; i < tmp.size(); i++) {
        values[i] = tmp[i];
    }
}

void fmi1_slave::get_boolean(const std::vector<value_ref>& vr, std::vector<bool>& values)
{
    auto tmp = std::vector<fmi1_boolean_t>(vr.size());
    fmi1_import_get_boolean(fmu_, vr.data(), vr.size(), tmp.data());
    for (auto i = 0; i < tmp.size(); i++) {
        values[i] = tmp[i] != 0;
    }
}

fmi1_slave::~fmi1_slave()
{
    fmi1_slave::freeInstance();
}

} // namespace fmi