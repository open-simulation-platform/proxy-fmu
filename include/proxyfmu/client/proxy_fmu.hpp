
#ifndef PROXY_PROXY_FMU_FMU_HPP
#define PROXY_PROXY_FMU_FMU_HPP

#include <proxyfmu/fmi/model_description.hpp>
#include <proxyfmu/fmi/slave.hpp>
#include <proxyfmu/fs_portability.hpp>
#include <proxyfmu/remote.hpp>

#include <optional>

namespace proxyfmu::client
{

class proxy_fmu
{

private:
    const filesystem::path& fmu_;
    const fmi::model_description modelDescription_;

    const std::optional<remote> remote_;

public:
    explicit proxy_fmu(const filesystem::path& fmu, std::optional<remote>  remote = std::nullopt);

    [[nodiscard]] const fmi::model_description& get_model_description() const;

    std::unique_ptr<fmi::slave> new_instance(const std::string& instanceName);

    ~proxy_fmu() = default;
};

} // namespace proxyfmu::client


#endif //PROXY_PROXY_FMU_FMU_HPP
