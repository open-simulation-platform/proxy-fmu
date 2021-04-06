
#ifndef PROXY_FMU_FMI1_SLAVE_HPP
#define PROXY_FMU_FMI1_SLAVE_HPP

#include "../../util/temp_dir.hpp"
#include "../fmicontext.hpp"

#include "fmi1_model_description.hpp"
#include <fmilib.h>
#include <proxyfmu/fmi/slave.hpp>

#include <memory>

namespace proxyfmu::fmi
{

class fmi1_slave : public slave
{
private:
    fmi1_import_t* handle_;
    const model_description md_;
    std::shared_ptr<fmicontext> ctx_;
    std::shared_ptr<util::temp_dir> tmpDir_;

    double start_time_;
    double stop_time_;

    bool freed = false;

public:
    fmi1_slave(
        const std::shared_ptr<fmicontext>& ctx,
        const std::string& instanceName,
        model_description md,
        std::shared_ptr<util::temp_dir> tmpDir);

    [[nodiscard]] const model_description &get_model_description() const override;
    void setup_experiment(double start_time, double stop_time, double /*tolerance*/) override;
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

    ~fmi1_slave() override;
};

} // namespace proxyfmu::fmi

#endif //PROXY_FMU_FMI1_SLAVE_HPP
