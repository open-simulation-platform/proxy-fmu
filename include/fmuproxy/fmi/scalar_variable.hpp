
#ifndef FMU_PROXY_SCALAR_VARIABLE_HPP
#define FMU_PROXY_SCALAR_VARIABLE_HPP

#include <optional>
#include <variant>
#include <string>
#include <vector>

namespace fmuproxy::fmi
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

    bool is_real()
    {
        return typeAttribute.index() == 0;
    }

    bool is_integer()
    {
        return typeAttribute.index() == 1;
    }

    bool is_string()
    {
        return typeAttribute.index() == 2;
    }

    bool is_boolean()
    {
        return typeAttribute.index() == 3;
    }
};

using model_variables = std::vector<scalar_variable>;

} // namespace fmuproxy::fmi

#endif //FMU_PROXY_SCALAR_VARIABLE_HPP
