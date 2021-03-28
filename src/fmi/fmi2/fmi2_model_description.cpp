
#include "fmi2_model_description.hpp"

namespace
{

std::optional<fmi::scalar_variable> to_scalar_variable(fmi2_import_variable_t* v)
{
    const auto type = fmi2_import_get_variable_base_type(v);
    if (type == fmi2_base_type_enum) {
        return std::nullopt;
    }

    fmi::scalar_variable var;
    var.vr = fmi2_import_get_variable_vr(v);
    var.name = fmi2_import_get_variable_name(v);

    switch (type) {
        case fmi2_base_type_real: {
            fmi::real r;
            if (fmi2_import_get_variable_has_start(v)) {
                r.start = fmi2_import_get_real_variable_start(fmi2_import_get_variable_as_real(v));
            }
            var.typeAttribute = r;
        } break;
        case fmi2_base_type_int: {
            fmi::integer i;
            if (fmi2_import_get_variable_has_start(v)) {
                i.start = fmi2_import_get_integer_variable_start(fmi2_import_get_variable_as_integer(v));
            }
            var.typeAttribute = i;
        } break;
        case fmi2_base_type_bool: {
            fmi::boolean b;
            if (fmi2_import_get_variable_has_start(v)) {
                b.start = fmi2_import_get_boolean_variable_start(fmi2_import_get_variable_as_boolean(v));
            }
            var.typeAttribute = b;
        } break;
        case fmi2_base_type_str: {
            fmi::string s;
            if (fmi2_import_get_variable_has_start(v)) {
                s.start = fmi2_import_get_string_variable_start(fmi2_import_get_variable_as_string(v));
            }
            var.typeAttribute = s;
        } break;
        case fmi2_base_type_enum: break;
    }
    return var;
}

} // namespace

namespace fmi
{

model_description create_model_description(fmi2_import_t* handle)
{
    model_description md;
    md.fmi_version = "1.0";
    md.guid = fmi2_import_get_GUID(handle);
    md.author = fmi2_import_get_author(handle);
    md.model_name = fmi2_import_get_model_name(handle);
    md.model_identifier = fmi2_import_get_model_identifier_CS(handle);
    md.description = fmi2_import_get_description(handle);
    md.generation_tool = fmi2_import_get_generation_tool(handle);
    md.generation_date_and_time = fmi2_import_get_generation_date_and_time(handle);

    md.default_experiment.start_time = fmi2_import_get_default_experiment_start(handle);
    md.default_experiment.stop_time = fmi2_import_get_default_experiment_stop(handle);
    md.default_experiment.step_size = fmi2_import_get_default_experiment_step(handle);
    md.default_experiment.tolerance = fmi2_import_get_default_experiment_tolerance(handle);

    const auto varList = fmi2_import_get_variable_list(handle, 0);
    const auto varCount = fmi2_import_get_variable_list_size(varList);
    for (auto i = 0; i < varCount; i++) {
        const auto var = fmi2_import_get_variable(varList, i);
        const auto scalar = to_scalar_variable(var);
        if (scalar) {
            md.model_variables.push_back(scalar.value());
        }
    }

    fmi2_import_free_variable_list(varList);

    return md;
}

} // namespace fmi