
#ifndef FMU_PROXY_THRIFT_SERVER_HELPER_HPP
#define FMU_PROXY_THRIFT_SERVER_HELPER_HPP

#include <fmuproxy/thrift/service_types.h>

#include <fmi4cpp/fmi4cpp.hpp>

using namespace fmi4cpp::fmi2;
using namespace fmuproxy::thrift;

namespace
{

Status::type thrift_type(fmi4cpp::status status)
{
    switch (status) {
        case fmi4cpp::status::OK:
            return Status::type::OK_STATUS;
        case fmi4cpp::status::Warning:
            return Status::type::WARNING_STATUS;
        case fmi4cpp::status::Pending:
            return Status::type::PENDING_STATUS;
        case fmi4cpp::status::Discard:
            return Status::type::DISCARD_STATUS;
        case fmi4cpp::status::Error:
            return Status::type::ERROR_STATUS;
        case fmi4cpp::status::Fatal:
            return Status::type::FATAL_STATUS;
        default:
            throw std::runtime_error("Fatal: Unknown status type!");
    }
}

template<typename T, typename U>
void set_scalar_variable_attributes(T t, const fmi4cpp::fmi2::scalar_variable_attribute<U>& a)
{
    if (a.start) {
        t.__set_start(*a.start);
    }
}

template<typename T, typename U>
void set_bounded_scalar_variable_attributes(T t, const fmi4cpp::fmi2::bounded_scalar_variable_attribute<U>& a)
{
    set_scalar_variable_attributes<T, U>(t, a);
    if (a.min) {
        t.__set_min(*a.min);
    }
    if (a.max) {
        t.__set_max(*a.max);
    }
    if (a.quantity) {
        t.__set_quantity(*a.quantity);
    }
}

fmuproxy::thrift::IntegerAttribute thrift_type(const fmi4cpp::fmi2::integer_attribute& a)
{
    fmuproxy::thrift::IntegerAttribute attribute;
    set_bounded_scalar_variable_attributes<fmuproxy::thrift::IntegerAttribute, int>(attribute, a);
    return attribute;
}

fmuproxy::thrift::RealAttribute thrift_type(const fmi4cpp::fmi2::real_attribute& a)
{
    fmuproxy::thrift::RealAttribute attribute;
    set_bounded_scalar_variable_attributes<fmuproxy::thrift::RealAttribute, double>(attribute, a);
    return attribute;
}

fmuproxy::thrift::StringAttribute thrift_type(const fmi4cpp::fmi2::string_attribute& a)
{
    fmuproxy::thrift::StringAttribute attribute;
    set_scalar_variable_attributes<fmuproxy::thrift::StringAttribute, std::string>(attribute, a);
    return attribute;
}

fmuproxy::thrift::BooleanAttribute thrift_type(const fmi4cpp::fmi2::boolean_attribute& a)
{
    fmuproxy::thrift::BooleanAttribute attribute;
    set_scalar_variable_attributes<fmuproxy::thrift::BooleanAttribute, bool>(attribute, a);
    return attribute;
}

fmuproxy::thrift::EnumerationAttribute thrift_type(const fmi4cpp::fmi2::enumeration_attribute& a)
{
    fmuproxy::thrift::EnumerationAttribute attribute;
    set_bounded_scalar_variable_attributes<fmuproxy::thrift::EnumerationAttribute, int>(attribute, a);
    return attribute;
}

fmuproxy::thrift::ScalarVariable thrift_type(const fmi4cpp::fmi2::scalar_variable& v)
{

    fmuproxy::thrift::ScalarVariable var;
    var.__set_name(v.name);
    var.__set_value_reference(v.value_reference);

    std::string description = v.description;
    if (!description.empty()) {
        var.__set_description(description);
    }

    var.__set_causality(fmi4cpp::fmi2::to_string(v.causality));
    var.__set_variability(fmi4cpp::fmi2::to_string(v.variability));
    var.__set_initial(fmi4cpp::fmi2::to_string(v.initial));

    if (v.is_integer()) {
        var.attribute.__set_integer_attribute(thrift_type(v.as_integer().attribute()));
    } else if (v.is_real()) {
        var.attribute.__set_real_attribute(thrift_type(v.as_real().attribute()));
    } else if (v.is_string()) {
        var.attribute.__set_string_attribute(thrift_type(v.as_string().attribute()));
    } else if (v.is_boolean()) {
        var.attribute.__set_boolean_attribute(thrift_type(v.as_boolean().attribute()));
    } else if (v.is_enumeration()) {
        var.attribute.__set_enumeration_attribute(thrift_type(v.as_enumeration().attribute()));
    } else {
        throw std::runtime_error("Fatal: No valid attribute found..");
    }

    return var;
}

void copy(fmuproxy::thrift::ModelVariables& variables, const fmi4cpp::fmi2::model_variables& mv)
{
    for (const auto& var : mv) {
        variables.push_back(thrift_type(var));
    }
}

void copy(fmuproxy::thrift::DefaultExperiment& to, const fmi4cpp::fmi2::default_experiment& from)
{
    if (from.startTime) {
        to.startTime = *from.startTime;
    }
    if (from.stopTime) {
        to.stopTime = *from.stopTime;
    }
    if (from.stepSize) {
        to.stepSize = *from.stepSize;
    }
    if (from.tolerance) {
        to.tolerance = *from.tolerance;
    }
}

void thrift_type(fmuproxy::thrift::ModelDescription& md, const fmi4cpp::fmi2::cs_model_description& m)
{

    md.__set_guid(m.guid);
    md.__set_fmi_version(m.fmi_version);
    md.__set_modelName(m.model_name);

    if (m.version) {
        md.__set_version(*m.version);
    }
    if (m.author) {
        md.__set_author(*m.author);
    }
    if (m.license) {
        md.__set_license(*m.license);
    }
    if (m.copyright) {
        md.__set_copyright(*m.copyright);
    }
    if (m.description) {
        md.__set_description(*m.description);
    }
    if (m.generation_tool) {
        md.__set_generation_tool(*m.generation_tool);
    }
    if (m.generation_date_and_time) {
        md.__set_generation_date_and_time(*m.generation_date_and_time);
    }
    if (m.variable_naming_convention) {
        md.__set_variable_naming_convention(*m.variable_naming_convention);
    }

    if (m.default_experiment) {
        fmuproxy::thrift::DefaultExperiment ex;
        copy(ex, *m.default_experiment);
        md.__set_default_experiment(ex);
    }

    fmuproxy::thrift::ModelVariables modelVariables;
    copy(modelVariables, *m.model_variables);
    md.__set_model_variables(modelVariables);

    md.__set_model_identifier(m.model_identifier);
    md.__set_can_get_and_set_fmu_state(m.can_get_and_set_fmu_state);
    md.__set_can_serialize_fmu_state(m.can_serialize_fmu_state);
    md.__set_can_interpolate_inputs(m.can_interpolate_inputs);
    md.__set_can_handle_variable_communication_step_size(m.can_handle_variable_communication_step_size);
    md.__set_provides_directional_derivative(m.provides_directional_derivative);
    md.__set_max_output_derivative_order(m.max_output_derivative_order);
}

} // namespace

#endif //FMU_PROXY_THRIFT_SERVER_HELPER_HPP
