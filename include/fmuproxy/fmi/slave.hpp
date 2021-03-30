
#ifndef FMU_PROXY_SLAVE_HPP
#define FMU_PROXY_SLAVE_HPP

#include <fmuproxy/fmi/model_description.hpp>

#include <memory>
#include <vector>

namespace fmuproxy::fmi
{

using value_ref = unsigned int;

class slave
{
public:
    virtual void setup_experiment(
        double start_time = 0,
        double stop_time = 0,
        double tolerance = 0) = 0;

    [[nodiscard]] virtual const model_description &get_model_description() const = 0;

    virtual void enter_initialization_mode() = 0;
    virtual void exit_initialization_mode() = 0;

    virtual void step(double current_time, double step_size) = 0;

    virtual void terminate() = 0;
    virtual void freeInstance() = 0;

    virtual void get_integer(const std::vector<value_ref>& vr, std::vector<int>& values) = 0;
    virtual void get_real(const std::vector<value_ref>& vr, std::vector<double>& values) = 0;
    virtual void get_string(const std::vector<value_ref>& vr, std::vector<std::string>& values) = 0;
    virtual void get_boolean(const std::vector<value_ref>& vr, std::vector<bool>& values) = 0;

    virtual void set_integer(const std::vector<value_ref>& vr, const std::vector<int>& values) = 0;
    virtual void set_real(const std::vector<value_ref>& vr, const std::vector<double>& values) = 0;
    virtual void set_string(const std::vector<value_ref>& vr, const std::vector<std::string>& values) = 0;
    virtual void set_boolean(const std::vector<value_ref>& vr, const std::vector<bool>& values) = 0;

    virtual ~slave() = default;
};

} // namespace fmuproxy::fmi

#endif //FMU_PROXY_SLAVE_HPP
