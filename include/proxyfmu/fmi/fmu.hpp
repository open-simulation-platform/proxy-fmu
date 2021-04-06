
#ifndef PROXY_FMU_FMU_HPP
#define PROXY_FMU_FMU_HPP

#include <filesystem>
#include <proxyfmu/fmi/model_description.hpp>
#include <proxyfmu/fmi/slave.hpp>
#include <memory>

namespace proxyfmu::fmi
{

class fmu
{

public:
    [[nodiscard]] virtual const model_description& get_model_description() const = 0;

    virtual std::unique_ptr<slave> new_instance(std::string instanceName) = 0;

    virtual ~fmu() = default;
};


std::unique_ptr<fmu> loadFmu(const std::filesystem::path& fmuPath);

} // namespace proxyfmu::fmi

#endif //PROXY_FMU_FMU_HPP
