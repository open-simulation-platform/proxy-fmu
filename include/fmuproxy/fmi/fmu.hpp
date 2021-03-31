
#ifndef FMU_PROXY_FMU_HPP
#define FMU_PROXY_FMU_HPP

#include <fmuproxy/fmi/model_description.hpp>
#include <fmuproxy/fmi/slave.hpp>

#include <filesystem>
#include <memory>

namespace fmuproxy::fmi
{

class fmu
{

public:
    [[nodiscard]] virtual const model_description& get_model_description() const = 0;

    virtual std::unique_ptr<slave> new_instance(std::string instanceName) = 0;

    virtual ~fmu() = default;
};


std::unique_ptr<fmu> loadFmu(const std::filesystem::path& fmuPath);

} // namespace fmuproxy::fmi

#endif //FMU_PROXY_FMU_HPP
