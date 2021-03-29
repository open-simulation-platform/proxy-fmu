
#include "fmi1_model_description.hpp"

namespace
{

std::optional<fmuproxy::fmi::scalar_variable> to_scalar_variable(fmi1_import_variable_t* v)
{
    const auto type = fmi1_import_get_variable_base_type(v);
    if (type == fmi1_base_type_enum) {
        return std::nullopt;
    }

    fmuproxy::fmi::scalar_variable var;
    var.vr = fmi1_import_get_variable_vr(v);
    var.name = fmi1_import_get_variable_name(v);

    switch (type) {
        case fmi1_base_type_real: {
            fmuproxy::fmi::real r;
            if (fmi1_import_get_variable_has_start(v)) {
                r.start = fmi1_import_get_real_variable_start(fmi1_import_get_variable_as_real(v));
            }
            var.typeAttribute = r;
        } break;
        case fmi1_base_type_int: {
            fmuproxy::fmi::integer i;
            if (fmi1_import_get_variable_has_start(v)) {
                i.start = fmi1_import_get_integer_variable_start(fmi1_import_get_variable_as_integer(v));
            }
            var.typeAttribute = i;
        } break;
        case fmi1_base_type_bool: {
            fmuproxy::fmi::boolean b;
            if (fmi1_import_get_variable_has_start(v)) {
                b.start = fmi1_import_get_boolean_variable_start(fmi1_import_get_variable_as_boolean(v));
            }
            var.typeAttribute = b;
        } break;
        case fmi1_base_type_str: {
            fmuproxy::fmi::string s;
            if (fmi1_import_get_variable_has_start(v)) {
                s.start = fmi1_import_get_string_variable_start(fmi1_import_get_variable_as_string(v));
            }
            var.typeAttribute = s;
        } break;
        case fmi1_base_type_enum: break;
    }
    return var;
}

} // namespace

namespace fmuproxy::fmi
{

model_description create_model_description(fmi1_import_t* handle)
{
    model_description md;
    md.fmi_version = "1.0";
    md.guid = fmi1_import_get_GUID(handle);
    md.author = fmi1_import_get_author(handle);
    md.model_name = fmi1_import_get_model_name(handle);
    md.model_identifier = fmi1_import_get_model_identifier(handle);
    md.description = fmi1_import_get_description(handle);
    md.generation_tool = fmi1_import_get_generation_tool(handle);
    md.generation_date_and_time = fmi1_import_get_generation_date_and_time(handle);

    md.default_experiment.start_time = fmi1_import_get_default_experiment_start(handle);
    md.default_experiment.stop_time = fmi1_import_get_default_experiment_stop(handle);
    md.default_experiment.tolerance = fmi1_import_get_default_experiment_tolerance(handle);

    const auto varList = fmi1_import_get_variable_list(handle);
    const auto varCount = fmi1_import_get_variable_list_size(varList);
    for (auto i = 0; i < varCount; i++) {
        const auto var = fmi1_import_get_variable(varList, i);
        const auto scalar = to_scalar_variable(var);
        if (scalar) {
            md.model_variables.push_back(scalar.value());
        }
    }

    fmi1_import_free_variable_list(varList);

    return md;
}

} // namespace fmuproxy::fmi
