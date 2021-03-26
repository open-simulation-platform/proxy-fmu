
#ifndef FMU_PROXY_FMU_HPP
#define FMU_PROXY_FMU_HPP

#include <fmi/model_description.hpp>
#include <fmi/slave.hpp>

#include <filesystem>
#include <memory>

namespace fmi
{

class fmu
{

public:
    virtual std::shared_ptr<model_description> get_model_description() = 0;

    virtual std::unique_ptr<slave> new_instance(std::string instanceName) = 0;

};


std::unique_ptr<fmu> loadFmu(const std::filesystem::path& fmuPath);

} // namespace fmi

#endif //FMU_PROXY_FMU_HPP
