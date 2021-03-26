
#include "fmi2_fmu.hpp"

#include "fmi2_slave.hpp"
#include <fmilib.h>

#include <exception>
#include <iostream>

namespace
{
void fmilogger(fmi2_component_t c, fmi2_string_t instanceName, fmi2_status_t status, fmi2_string_t category, fmi2_string_t message, ...)
{
    va_list argp;
    va_start(argp, message);
    fmi2_log_forwarding_v(c, instanceName, status, category, message, argp);
    va_end(argp);
}
} // namespace

namespace fmi
{

fmi2_fmu::fmi2_fmu(fmi_import_context_t* ctx, std::shared_ptr<temp_dir> tmpDir)
    : ctx_(ctx)
    , handle_(fmi2_import_parse_xml(ctx, tmpDir->path().string().c_str(), nullptr))
    , md_(std::make_shared<fmi2_model_description>(handle_))
    , tmpDir_(std::move(tmpDir))
{
    if (fmi2_import_get_fmu_kind(handle_) != fmi2_fmu_kind_cs) {
        throw std::runtime_error("FMU does not support Co-simulation!");
    }

    fmi2_callback_functions_t callbackFunctions;
    callbackFunctions.allocateMemory = std::calloc;
    callbackFunctions.freeMemory = std::free;
    callbackFunctions.logger = fmilogger;
    callbackFunctions.componentEnvironment = nullptr;

    if (fmi2_import_create_dllfmu(handle_, fmi2_fmu_kind_cs, &callbackFunctions) != jm_status_success) {
        throw std::runtime_error(std::string("failed to load fmu dll! Error: ") + fmi2_import_get_last_error(handle_));
    }
}

std::shared_ptr<model_description> fmi2_fmu::get_model_description()
{
    return md_;
}

std::unique_ptr<slave> fmi2_fmu::new_instance(std::string instanceName)
{
    return std::make_unique<fmi2_slave>(handle_, instanceName, md_, tmpDir_);
}

fmi2_fmu::~fmi2_fmu()
{
    fmi_import_free_context(ctx_);
}

} // namespace fmi
