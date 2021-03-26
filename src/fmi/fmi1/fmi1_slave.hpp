
#ifndef FMU_PROXY_FMI1_SLAVE_HPP
#define FMU_PROXY_FMI1_SLAVE_HPP

#include "../temp_dir.hpp"

#include "fmi1_model_description.hpp"
#include <fmi/slave.hpp>
#include <fmilib.h>

#include <memory>

namespace fmi
{

class fmi1_slave : public slave
{
private:
    fmi1_import_t* fmu_;
    std::shared_ptr<temp_dir> tmpDir_;
    std::shared_ptr<fmi1_model_description> md_;

    double start_time_;
    double stop_time_;

    bool freed = false;

public:
    fmi1_slave(
        fmi1_import_t* fmu,
        const std::string& instanceName,
        std::shared_ptr<fmi1_model_description> md,
        std::shared_ptr<temp_dir> tmpDir);

    std::shared_ptr<model_description> get_model_description() override;
    void setup_experiment(double start_time, double stop_time, double /*tolerance*/) override;
    void enter_initialization_mode() override;
    void exit_initialization_mode() override;
    void step(double current_time, double step_size) override;
    void terminate() override;
    void freeInstance() override;
    ~fmi1_slave();
};

} // namespace fmi

#endif //FMU_PROXY_FMI1_SLAVE_HPP
