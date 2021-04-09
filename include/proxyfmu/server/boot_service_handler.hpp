
#ifndef PROXYFMU_BOOT_SERVICE_HANDLER_HPP
#define PROXYFMU_BOOT_SERVICE_HANDLER_HPP

#include <proxyfmu/thrift/BootService.h>

namespace proxyfmu::server
{

class boot_service_handler : virtual public thrift::BootServiceIf
{

public:
    boot_service_handler() = default;
    int64_t loadFromBinaryData(const std::string& name, const std::string& data) override;
};

} // namespace proxyfmu

#endif //PROXYFMU_BOOT_SERVICE_HANDLER_HPP
