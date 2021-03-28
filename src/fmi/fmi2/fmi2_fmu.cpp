
#include "fmi2_fmu.hpp"
#include "fmi2_model_description.hpp"
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

fmi2_fmu::fmi2_fmu(std::unique_ptr<fmicontext> ctx, std::shared_ptr<temp_dir> tmpDir)
    : ctx_(std::move(ctx))
    , handle_(fmi2_import_parse_xml(ctx->ctx_, tmpDir->path().string().c_str(), nullptr))
    , md_(create_model_description(handle_))
    , tmpDir_(std::move(tmpDir))
{
    if (fmi2_import_get_fmu_kind(handle_) != fmi2_fmu_kind_cs) {
        throw std::runtime_error("FMU does not support Co-simulation!");
    }

    fmi2_callback_functions_t callbackFunctions;
    callbackFunctions.allocateMemory = calloc;
    callbackFunctions.freeMemory = free;
    callbackFunctions.logger = &fmilogger;
    callbackFunctions.componentEnvironment = nullptr;

    if (fmi2_import_create_dllfmu(handle_, fmi2_fmu_kind_cs, &callbackFunctions) != jm_status_success) {
        throw std::runtime_error(std::string("failed to load fmu dll! Error: ") + fmi2_import_get_last_error(handle_));
    }
}

const model_description& fmi2_fmu::get_model_description() const
{
    return md_;
}

std::unique_ptr<slave> fmi2_fmu::new_instance(std::string instanceName)
{
    return std::make_unique<fmi2_slave>(handle_, instanceName, md_, tmpDir_);
}

fmi2_fmu::~fmi2_fmu()
{
    fmi2_import_destroy_dllfmu(handle_);
    fmi2_import_free(handle_);
}


} // namespace fmi
