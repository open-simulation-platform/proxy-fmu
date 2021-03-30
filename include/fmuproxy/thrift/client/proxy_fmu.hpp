
#ifndef FMU_PROXY_PROXY_FMU_HPP
#define FMU_PROXY_PROXY_FMU_HPP

#include <fmuproxy/fmi/model_description.hpp>
#include <fmuproxy/fmi/slave.hpp>

#include <filesystem>

namespace fmuproxy::client
{

class proxy_fmu
{

private:
    const std::filesystem::path& fmu_;
    const fmi::model_description modelDescription_;

public:
    explicit proxy_fmu(const std::filesystem::path& fmu);

    [[nodiscard]] const fmi::model_description& get_model_description() const;

    std::unique_ptr<fmi::slave> new_instance(const std::string& instanceName);

    ~proxy_fmu() = default;
};

} // namespace fmuproxy::client


#endif //FMU_PROXY_PROXY_FMU_HPP
