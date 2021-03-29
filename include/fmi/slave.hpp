
#ifndef FMU_PROXY_SLAVE_HPP
#define FMU_PROXY_SLAVE_HPP

#include <memory>
#include <vector>

using value_ref = unsigned int;

namespace fmi
{

class slave
{
public:
    virtual void setup_experiment(
        double start_time = 0,
        double stop_time = 0,
        double tolerance = 0) = 0;

    [[nodiscard]] virtual model_description get_model_description() const = 0;

    virtual void enter_initialization_mode() = 0;
    virtual void exit_initialization_mode() = 0;

    virtual void step(double current_time, double step_size) = 0;

    virtual void terminate() = 0;
    virtual void freeInstance() = 0;

    virtual void get_integer(const std::vector<value_ref> &vr, std::vector<int> &values) = 0;
    virtual void get_real(const std::vector<value_ref> &vr, std::vector<double> &values) = 0;
    virtual void get_string(const std::vector<value_ref> &vr, std::vector<std::string> &values) = 0;
    virtual void get_boolean(const std::vector<value_ref> &vr, std::vector<bool> &values) = 0;

    virtual ~slave() = default;
};

} // namespace fmi

#endif //FMU_PROXY_SLAVE_HPP
