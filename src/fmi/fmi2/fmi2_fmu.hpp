
#ifndef FMU_PROXY_FMI2_FMU_HPP
#define FMU_PROXY_FMI2_FMU_HPP


#include "../fmicontext.hpp"
#include "../temp_dir.hpp"

#include <fmi/fmu.hpp>
#include <fmilib.h>

namespace fmi
{

class fmi2_fmu : public fmu
{

private:
    fmi2_import_t* handle_;
    std::unique_ptr<fmicontext> ctx_;

    const model_description md_;
    std::shared_ptr<temp_dir> tmpDir_;

public:
    fmi2_fmu(std::unique_ptr<fmicontext> ctx, std::shared_ptr<temp_dir> tmpDir);

    [[nodiscard]] const model_description& get_model_description() const override;

    std::unique_ptr<slave> new_instance(std::string instanceName) override;

    ~fmi2_fmu() override;
};

} // namespace fmi

#endif //FMU_PROXY_FMI2_FMU_HPP