
#include "remote_slave.hpp"

#include <proxyfmu/client/proxy_fmu.hpp>
#include <proxyfmu/fmi/fmu.hpp>

#include <memory>
#include <utility>


namespace proxyfmu::client
{

proxy_fmu::proxy_fmu(const filesystem::path& fmu, std::optional<remote_info> remote)
    : fmu_(fmu)
    , remote_(std::move(remote))
    , modelDescription_(fmi::loadFmu(fmu)->get_model_description())
{
}

const fmi::model_description& proxy_fmu::get_model_description() const
{
    return modelDescription_;
}

std::unique_ptr<fmi::slave> proxy_fmu::new_instance(const std::string& instanceName)
{
    return std::make_unique<remote_slave>(fmu_, instanceName, modelDescription_, remote_);
}

} // namespace proxyfmu::client
