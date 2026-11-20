
#ifndef PROXY_FMU_SLAVE_HPP
#define PROXY_FMU_SLAVE_HPP

#include <proxyfmu/fmi/model_description.hpp>
#include <proxyfmu/state.hpp>

#include <memory>
#include <string>
#include <vector>

namespace proxyfmu::fmi
{

using namespace proxyfmu::state;
using value_ref = unsigned int;

/// Value references for variables of every type, used by the batched operations.
struct variable_refs
{
    std::vector<value_ref> integers;
    std::vector<value_ref> reals;
    std::vector<value_ref> strings;
    std::vector<value_ref> booleans;

    [[nodiscard]] bool empty() const
    {
        return integers.empty() && reals.empty() && strings.empty() && booleans.empty();
    }
};

/// Values for variables of every type, ordered as in the matching `variable_refs`.
struct variable_values
{
    std::vector<int> integers;
    std::vector<double> reals;
    std::vector<std::string> strings;
    std::vector<bool> booleans;
};

class slave
{
public:
    virtual bool setup_experiment(
        double start_time = 0,
        double stop_time = 0,
        double tolerance = 0) = 0;

    [[nodiscard]] virtual const model_description& get_model_description() const = 0;

    virtual bool enter_initialization_mode() = 0;
    virtual bool exit_initialization_mode() = 0;

    virtual bool step(double current_time, double step_size) = 0;

    virtual bool terminate() = 0;
    virtual void freeInstance() = 0;

    virtual bool get_integer(const std::vector<value_ref>& vr, std::vector<int>& values) = 0;
    virtual bool get_real(const std::vector<value_ref>& vr, std::vector<double>& values) = 0;
    virtual bool get_string(const std::vector<value_ref>& vr, std::vector<std::string>& values) = 0;
    virtual bool get_boolean(const std::vector<value_ref>& vr, std::vector<bool>& values) = 0;

    virtual bool set_integer(const std::vector<value_ref>& vr, const std::vector<int>& values) = 0;
    virtual bool set_real(const std::vector<value_ref>& vr, const std::vector<double>& values) = 0;
    virtual bool set_string(const std::vector<value_ref>& vr, const std::vector<std::string>& values) = 0;
    virtual bool set_boolean(const std::vector<value_ref>& vr, const std::vector<bool>& values) = 0;

    /// Gets variables of every type in one operation. Types with no references are skipped.
    virtual bool get_all(const variable_refs& vr, variable_values& values)
    {
        values.integers.resize(vr.integers.size());
        values.reals.resize(vr.reals.size());
        values.strings.resize(vr.strings.size());
        values.booleans.resize(vr.booleans.size());
        return (vr.integers.empty() || get_integer(vr.integers, values.integers)) &&
            (vr.reals.empty() || get_real(vr.reals, values.reals)) &&
            (vr.strings.empty() || get_string(vr.strings, values.strings)) &&
            (vr.booleans.empty() || get_boolean(vr.booleans, values.booleans));
    }

    /// Sets variables of every type in one operation. Types with no references are skipped.
    virtual bool set_all(const variable_refs& vr, const variable_values& values)
    {
        return (vr.integers.empty() || set_integer(vr.integers, values.integers)) &&
            (vr.reals.empty() || set_real(vr.reals, values.reals)) &&
            (vr.strings.empty() || set_string(vr.strings, values.strings)) &&
            (vr.booleans.empty() || set_boolean(vr.booleans, values.booleans));
    }

    /**
     * Sets `setValues`, performs a time step and then gets `getValues`, in that order.
     * Stops at, and returns false on, the first operation that fails.
     *
     * For an out-of-process slave this is a single round trip, where the
     * individual calls would be up to nine.
     */
    virtual bool step_exchange(
        const variable_refs& setVr,
        const variable_values& setValues,
        double current_time,
        double step_size,
        const variable_refs& getVr,
        variable_values& getValues)
    {
        return set_all(setVr, setValues) &&
            step(current_time, step_size) &&
            get_all(getVr, getValues);
    }

    virtual state_index save_state() = 0;
    virtual void save_state(state_index stateIndex) = 0;
    virtual void restore_state(state_index stateIndex) = 0;
    virtual void release_state(state_index stateIndex) = 0;
    virtual void export_state(state_index stateIndex, state::exported_state& exportedState) const = 0;
    virtual state_index import_state(const state::exported_state& exportedState) = 0;

    virtual ~slave() = default;
};

} // namespace proxyfmu::fmi

#endif // PROXY_FMU_SLAVE_HPP
