
#ifndef PROXY_FMU_FMI1_FMU_HPP
#define PROXY_FMU_FMI1_FMU_HPP

#include "../temp_dir.hpp"
#include "../fmicontext.hpp"

#include <fmilib.h>
#include <proxyfmu/fmi/fmu.hpp>

namespace proxyfmu::fmi
{

class fmi1_fmu : public fmu
{

private:
    fmi1_import_t* handle_;
    std::shared_ptr<fmicontext> ctx_;

    const model_description md_;
    std::shared_ptr<util::temp_dir> tmpDir_;

public:
    fmi1_fmu(std::unique_ptr<fmicontext> ctx, std::shared_ptr<util::temp_dir> tmpDir);

    [[nodiscard]] const model_description& get_model_description() const override;

    std::unique_ptr<slave> new_instance(std::string instanceName) override;

    ~fmi1_fmu() override;
};

} // namespace proxyfmu::fmi

#endif //PROXY_FMU_FMI2_FMU_HPP