
#ifndef PROXY_FMU_FMI1_SLAVE_HPP
#define PROXY_FMU_FMI1_SLAVE_HPP

#include "../fmicontext.hpp"

#include "fmi1_model_description.hpp"
#include <fmilib.h>
#include <proxyfmu/fmi/slave.hpp>
#include <proxyfmu/temp_dir.hpp>
#include <proxyfmu/thrift/defs_types.h>

#include <memory>

namespace proxyfmu::fmi
{

class fmi1_slave : public slave
{
private:
    fmi1_import_t* handle_;
    const model_description md_;
    std::shared_ptr<fmicontext> ctx_;
    std::shared_ptr<temp_dir> tmpDir_;

    double start_time_;
    double stop_time_;

    bool freed = false;

public:
    fmi1_slave(
        const std::shared_ptr<fmicontext>& ctx,
        const std::string& instanceName,
        model_description md,
        std::shared_ptr<temp_dir> tmpDir);

    [[nodiscard]] const model_description& get_model_description() const override;
    bool setup_experiment(double start_time, double stop_time, double /*tolerance*/) override;
    bool enter_initialization_mode() override;
    bool exit_initialization_mode() override;
    bool step(double current_time, double step_size) override;
    bool terminate() override;
    void freeInstance() override;

    bool get_integer(const std::vector<value_ref>& vr, std::vector<int>& values) override;
    bool get_real(const std::vector<value_ref>& vr, std::vector<double>& values) override;
    bool get_string(const std::vector<value_ref>& vr, std::vector<std::string>& values) override;
    bool get_boolean(const std::vector<value_ref>& vr, std::vector<bool>& values) override;

    bool set_integer(const std::vector<value_ref>& vr, const std::vector<int>& values) override;
    bool set_real(const std::vector<value_ref>& vr, const std::vector<double>& values) override;
    bool set_string(const std::vector<value_ref>& vr, const std::vector<std::string>& values) override;
    bool set_boolean(const std::vector<value_ref>& vr, const std::vector<bool>& values) override;

    state_index save_state() override;
    void save_state(state_index stateIndex) override;
    void restore_state(state_index stateIndex) override;
    void release_state(state_index stateIndex) override;
    void export_state(state_index stateIndex, proxyfmu::thrift::ExportedState& es) const override;
    state_index import_state(const proxyfmu::thrift::ExportedState& exportedState) override;

    ~fmi1_slave() override;
};

} // namespace proxyfmu::fmi

#endif // PROXY_FMU_FMI1_SLAVE_HPP
