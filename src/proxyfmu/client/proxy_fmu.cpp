
#include "proxy_slave.hpp"

#include <proxyfmu/client/proxy_fmu.hpp>
#include <proxyfmu/fmi/fmu.hpp>

#include <memory>
#include <utility>


namespace proxyfmu::client
{

proxy_fmu::proxy_fmu(const filesystem::path& fmuPath, std::optional<remote_info> remote)
    : fmuPath_(fmuPath)
    , remote_(std::move(remote))
    , modelDescription_(fmi::loadFmu(fmuPath)->get_model_description())
{
}

const fmi::model_description& proxy_fmu::get_model_description() const
{
    return modelDescription_;
}

std::unique_ptr<fmi::slave> proxy_fmu::new_instance(const std::string& instanceName)
{
    return std::make_unique<proxy_slave>(fmuPath_, instanceName, modelDescription_, remote_);
}

} // namespace proxyfmu::client
