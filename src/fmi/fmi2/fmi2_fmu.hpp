
#ifndef FMU_PROXY_FMI2_FMU_HPP
#define FMU_PROXY_FMI2_FMU_HPP

#include "fmi2_model_description.hpp"
#include "../temp_dir.hpp"
#include <fmi/fmu.hpp>
#include <fmilib.h>

namespace fmi
{

class fmi2_fmu : public fmu
{

private:

    fmi2_import_t* handle_;
    fmi_import_context_t* ctx_;

    std::shared_ptr<temp_dir> tmpDir_;
    std::shared_ptr<fmi2_model_description> md_;


public:
    fmi2_fmu(fmi_import_context_t* ctx, std::shared_ptr<temp_dir> tmpDir);

    std::shared_ptr<model_description> get_model_description() override;

    std::unique_ptr<slave> new_instance(std::string instanceName) override;

    ~fmi2_fmu();
};

} // namespace fmi

#endif //FMU_PROXY_FMI2_FMU_HPP