
#include "remote_slave.hpp"

#include <proxyfmu/client/proxy_fmu.hpp>
#include <proxyfmu/fmi/fmu.hpp>

#include <memory>


namespace proxyfmu::client
{

proxy_fmu::proxy_fmu(const std::filesystem::path& fmu)
    : fmu_(fmu)
    , modelDescription_(fmi::loadFmu(fmu)->get_model_description())
{
}

const fmi::model_description& proxy_fmu::get_model_description() const
{
    return modelDescription_;
}

std::unique_ptr<fmi::slave> proxy_fmu::new_instance(const std::string& instanceName)
{
    return std::make_unique<remote_slave>(fmu_, instanceName, modelDescription_);
}

} // namespace proxyfmu::client
