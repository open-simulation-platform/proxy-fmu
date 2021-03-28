
#ifndef FMU_PROXY_SCALAR_VARIABLE_HPP
#define FMU_PROXY_SCALAR_VARIABLE_HPP

#include <variant>

namespace fmi
{

struct real
{
    std::optional<double> start;
};

struct integer
{
    std::optional<int> start;
};

struct string
{
    std::optional<std::string> start;
};

struct boolean
{
    std::optional<bool> start;
};

using type_attribute = std::variant<real, integer, string, boolean>;

struct scalar_variable
{
    unsigned int vr;
    std::string name;
    std::optional<std::string> initial;
    std::optional<std::string> causality;
    std::optional<std::string> variability;
    type_attribute typeAttribute;
};

using model_variables = std::vector<scalar_variable>;

} // namespace fmi

#endif //FMU_PROXY_SCALAR_VARIABLE_HPP
