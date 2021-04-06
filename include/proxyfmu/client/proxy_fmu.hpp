
#ifndef PROXY_PROXY_FMU_FMU_HPP
#define PROXY_PROXY_FMU_FMU_HPP

#include <proxyfmu/fmi/model_description.hpp>
#include <proxyfmu/fmi/slave.hpp>
#include <proxyfmu/fs_portability.hpp>

namespace proxyfmu::client
{

class proxy_fmu
{

private:
    const filesystem::path& fmu_;
    const fmi::model_description modelDescription_;

public:
    explicit proxy_fmu(const filesystem::path& fmu);

    [[nodiscard]] const fmi::model_description& get_model_description() const;

    std::unique_ptr<fmi::slave> new_instance(const std::string& instanceName);

    ~proxy_fmu() = default;
};

} // namespace proxyfmu::client


#endif //PROXY_PROXY_FMU_FMU_HPP
