#include "fmi1_fmu.hpp"

#include <fmilib.h>
#include "fmi1_slave.hpp"

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

namespace fmi
{

fmi1_fmu::fmi1_fmu(fmi_import_context_t* ctx, std::shared_ptr<temp_dir> tmpDir)
    : ctx_(ctx)
    , handle_(fmi1_import_parse_xml(ctx, tmpDir->path().string().c_str()))
    , tmpDir_(std::move(tmpDir))
    , md_(std::make_shared<fmi1_model_description>(handle_))
{
    auto kind = fmi1_import_get_fmu_kind(handle_);
    if (kind != fmi1_fmu_kind_enu_cs_standalone && kind != fmi1_fmu_kind_enu_cs_tool) {
        throw std::runtime_error("FMU does not support Co-simulation!");
    }

    fmi1_callback_functions_t callbackFunctions;
    callbackFunctions.allocateMemory = std::calloc;
    callbackFunctions.freeMemory = std::free;
    callbackFunctions.logger = fmilogger;

    if (fmi1_import_create_dllfmu(handle_, callbackFunctions, 1) != jm_status_success) {
        throw std::runtime_error(std::string("failed to load fmu dll! Error: ") + fmi1_import_get_last_error(handle_));
    }
}

std::shared_ptr<model_description> fmi1_fmu::get_model_description()
{
    return md_;
}

std::unique_ptr<slave> fmi1_fmu::new_instance(std::string instanceName)
{
    return std::make_unique<fmi1_slave>(handle_, instanceName, md_, tmpDir_);
}

fmi1_fmu::~fmi1_fmu()
{
    fmi_import_free_context(ctx_);
}

} // namespace fmi