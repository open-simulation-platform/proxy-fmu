
#ifndef FMU_PROXY_MODEL_DESCRIPTION_HPP
#define FMU_PROXY_MODEL_DESCRIPTION_HPP

#include <vector>
#include <optional>

#include <fmi/scalar_variable.hpp>

namespace fmi
{

struct Unknown
{
    int index;
    std::vector<int> dependencies;
    std::vector<std::string> dependencies_kind;
};

struct default_experiment
{
    std::optional<double> start_time;
    std::optional<double> stop_time;
    std::optional<double> tolerance;
    std::optional<double> step_size;
};

class model_description
{

public:
    virtual std::string get_guid() = 0;
    virtual std::string get_author() = 0;
    virtual std::string get_model_name() = 0;
    virtual std::string get_fmi_version() = 0;
    virtual std::string get_description() = 0;
    virtual std::string get_generation_tool() = 0;
    virtual std::string get_generation_date_and_time() = 0;
    virtual std::optional<default_experiment> get_default_experiment() = 0;
    virtual model_variables get_model_variables() = 0;
};

} // namespace fmi

#endif //FMU_PROXY_MODEL_DESCRIPTION_HPP
