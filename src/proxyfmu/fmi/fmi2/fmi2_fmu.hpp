
#ifndef PROXY_FMU_FMI2_FMU_HPP
#define PROXY_FMU_FMI2_FMU_HPP

#include "../fmicontext.hpp"

#include <fmilib.h>
#include <proxyfmu/fmi/fmu.hpp>
#include <proxyfmu/temp_dir.hpp>

namespace proxyfmu::fmi
{

class fmi2_fmu : public fmu
{

private:
    fmi2_import_t* handle_;
    std::shared_ptr<fmicontext> ctx_;

    const model_description md_;
    std::shared_ptr<temp_dir> tmpDir_;

public:
    fmi2_fmu(std::shared_ptr<fmicontext> ctx, std::shared_ptr<temp_dir> tmpDir);

    [[nodiscard]] const model_description& get_model_description() const override;

    std::unique_ptr<slave> new_instance(const std::string& instanceName) override;

    ~fmi2_fmu() override;
};

} // namespace proxyfmu::fmi

#endif // PROXY_FMU_FMI2_FMU_HPP