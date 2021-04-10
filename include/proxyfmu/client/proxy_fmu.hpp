
#ifndef PROXY_PROXY_FMU_FMU_HPP
#define PROXY_PROXY_FMU_FMU_HPP

#include <proxyfmu/fmi/model_description.hpp>
#include <proxyfmu/fmi/slave.hpp>
#include <proxyfmu/fmi/fmu.hpp>
#include <proxyfmu/fs_portability.hpp>
#include <proxyfmu/remote_info.hpp>

#include <optional>

namespace proxyfmu::client
{

class proxy_fmu: public fmi::fmu
{

private:
    const filesystem::path fmuPath_;
    const fmi::model_description modelDescription_;

    const std::optional<remote_info> remote_;

public:
    explicit proxy_fmu(const filesystem::path& fmuPath, std::optional<remote_info> remote = std::nullopt);

    [[nodiscard]] const fmi::model_description& get_model_description() const override;

    std::unique_ptr<fmi::slave> new_instance(const std::string& instanceName) override;

    ~proxy_fmu() override = default;
};

} // namespace proxyfmu::client


#endif //PROXY_PROXY_FMU_FMU_HPP
