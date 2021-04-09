
#ifndef PROXYFMU_BOOT_SERVICE_HANDLER_HPP
#define PROXYFMU_BOOT_SERVICE_HANDLER_HPP

#include <proxyfmu/temp_dir.hpp>
#include <proxyfmu/thrift/BootService.h>

#include <vector>
#include <memory>

namespace proxyfmu::server
{

class boot_service_handler : virtual public thrift::BootServiceIf
{

private:
    std::vector<std::unique_ptr<temp_dir>> dirs_;

public:
    boot_service_handler() = default;
    int64_t loadFromBinaryData(const std::string& fmuName, const std::string& instanceName, const std::string& data) override;
};

} // namespace proxyfmu::server

#endif //PROXYFMU_BOOT_SERVICE_HANDLER_HPP
