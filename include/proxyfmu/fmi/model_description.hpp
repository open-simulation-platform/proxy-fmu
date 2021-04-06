
#ifndef PROXY_FMU_MODEL_DESCRIPTION_HPP
#define PROXY_FMU_MODEL_DESCRIPTION_HPP

#include <proxyfmu/fmi/scalar_variable.hpp>

#include <optional>
#include <vector>

namespace proxyfmu::fmi
{

struct default_experiment
{
    double start_time;
    double stop_time;
    double tolerance;
    double step_size;
};

struct model_description
{
    std::string guid;
    std::string author;
    std::string model_name;
    std::string model_identifier;
    std::string fmi_version;
    std::string description;
    std::string generation_tool;
    std::string generation_date_and_time;

    model_variables model_variables;
    default_experiment default_experiment;
};

} // namespace proxyfmu::fmi

#endif //PROXY_FMU_MODEL_DESCRIPTION_HPP
