
#include "fmi2_slave.hpp"

#include <fmilib.h>

#include <exception>
#include <utility>
#include <memory>

namespace
{
void fmilogger(fmi2_component_t c, fmi2_string_t instanceName, fmi2_status_t status, fmi2_string_t category, fmi2_string_t message, ...)
{
    //TODO
}



void logger_callback(
    jm_callbacks* /*callbacks*/,
    jm_string module,
    jm_log_level_enu_t logLevel,
    jm_string message)
{
    printf("module = %s, log level = %d: %s\n", module, logLevel, message);
}

} // namespace

namespace fmi
{

fmi2_slave::fmi2_slave(
    fmi2_import_t* fmu,
    const std::string& instanceName,
    std::shared_ptr<fmi2_model_description> md,
    std::shared_ptr<temp_dir> tmpDir)
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

std::shared_ptr<model_description> fmi2_slave::get_model_description()
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
        fmi2_import_destroy_dllfmu(fmu_);
        fmi2_import_free(fmu_);
    }
}

fmi2_slave::~fmi2_slave()
{
    //fmi2_slave::freeInstance();
}

} // namespace fmi