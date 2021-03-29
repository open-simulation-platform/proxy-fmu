
#ifndef FMU_PROXY_FMI2_SLAVE_HPP
#define FMU_PROXY_FMI2_SLAVE_HPP

#include "../../util/temp_dir.hpp"

#include "fmi2_model_description.hpp"
#include <fmilib.h>
#include <fmuproxy/fmi/slave.hpp>

#include <memory>

namespace fmuproxy::fmi
{

class fmi2_slave : public slave
{
private:
    fmi2_import_t* fmu_;
    model_description md_;
    std::shared_ptr<fmuproxy::util::temp_dir> tmpDir_;

    bool freed = false;

public:
    fmi2_slave(
        fmi2_import_t* fmu,
        const std::string& instanceName,
        model_description md,
        std::shared_ptr<fmuproxy::util::temp_dir> tmpDir);

    [[nodiscard]] model_description get_model_description() const override;
    void setup_experiment(double start_time, double stop_time, double tolerance) override;

    void enter_initialization_mode() override;
    void exit_initialization_mode() override;
    void step(double current_time, double step_size) override;

    void terminate() override;
    void freeInstance() override;

    void get_integer(const std::vector<value_ref>& vr, std::vector<int>& values) override;
    void get_real(const std::vector<value_ref>& vr, std::vector<double>& values) override;
    void get_string(const std::vector<value_ref>& vr, std::vector<std::string>& values) override;
    void get_boolean(const std::vector<value_ref>& vr, std::vector<bool>& values) override;

    void set_integer(const std::vector<value_ref>& vr, const std::vector<int>& values) override;
    void set_real(const std::vector<value_ref>& vr, const std::vector<double>& values) override;
    void set_string(const std::vector<value_ref>& vr, const std::vector<std::string>& values) override;
    void set_boolean(const std::vector<value_ref>& vr, const std::vector<bool>& values) override;

    ~fmi2_slave() override;
};

} // namespace fmi

#endif //FMU_PROXY_FMI2_SLAVE_HPP
