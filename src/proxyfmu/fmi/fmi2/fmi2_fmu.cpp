
#include "fmi2_fmu.hpp"

#include "fmi2_model_description.hpp"
#include "fmi2_slave.hpp"
#include <fmilib.h>

#include <exception>
#include <iostream>

namespace proxyfmu::fmi
{

fmi2_fmu::fmi2_fmu(std::shared_ptr<fmicontext> ctx, std::shared_ptr<temp_dir> tmpDir)
    : ctx_(std::move(ctx))
    , handle_(fmi2_import_parse_xml(ctx->ctx_, tmpDir->path().string().c_str(), nullptr))
    , md_(create_model_description(handle_))
    , tmpDir_(std::move(tmpDir))
{
    if (fmi2_import_get_fmu_kind(handle_) != fmi2_fmu_kind_cs) {
        throw std::runtime_error("FMU does not support Co-simulation!");
    }
}

const model_description& fmi2_fmu::get_model_description() const
{
    return md_;
}

std::unique_ptr<slave> fmi2_fmu::new_instance(const std::string& instanceName)
{
    return std::make_unique<fmi2_slave>(ctx_, instanceName, md_, tmpDir_);
}

fmi2_fmu::~fmi2_fmu()
{
    fmi2_import_free(handle_);
}


} // namespace proxyfmu::fmi
