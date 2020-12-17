
#ifndef FMU_PROXY_THRIFT_CLIENT_HELPER_HPP
#define FMU_PROXY_THRIFT_CLIENT_HELPER_HPP

#include <fmuproxy/thrift/service_types.h>

#include <fmi4cpp/fmi2/fmi2.hpp>

#include <iostream>
#include <memory>
#include <optional>

using namespace fmuproxy::thrift;

namespace
{

fmi4cpp::status convert(const Status::type status)
{
    switch (status) {
        case Status::OK_STATUS:
            return fmi4cpp::status::OK;
        case Status::WARNING_STATUS:
            return fmi4cpp::status::Warning;
        case Status::DISCARD_STATUS:
            return fmi4cpp::status::Discard;
        case Status::ERROR_STATUS:
            return fmi4cpp::status::Error;
        case Status::FATAL_STATUS:
            return fmi4cpp::status::Fatal;
        case Status::PENDING_STATUS:
            return fmi4cpp::status::Pending;
        default:
            return fmi4cpp::status::Unknown;
    }
}

fmi4cpp::fmi2::default_experiment convert(const fmuproxy::thrift::DefaultExperiment& d)
{
    return {d.startTime, d.stopTime, d.stepSize, d.tolerance};
}

template<typename T, typename U>
fmi4cpp::fmi2::scalar_variable_attribute<T> to_scalar_variable_attributes(const U& a)
{
    if (a.__isset.start) {
        auto attribute = fmi4cpp::fmi2::scalar_variable_attribute<T>();
        attribute.start = a.start;
        return attribute;
    } else {
        return {};
    }
}

template<typename T, typename U>
fmi4cpp::fmi2::bounded_scalar_variable_attribute<T> to_bounded_scalar_variable_attributes(const U& a)
{

    fmi4cpp::fmi2::bounded_scalar_variable_attribute<T> bounded(to_scalar_variable_attributes<T, U>(a));
    if (a.__isset.min) {
        bounded.min = a.min;
    }
    if (a.__isset.max) {
        bounded.max = a.max;
    }
    if (a.__isset.quantity) {
        bounded.quantity = a.quantity;
    }

    return bounded;
}

fmi4cpp::fmi2::integer_attribute convert(const fmuproxy::thrift::IntegerAttribute& a)
{
    return fmi4cpp::fmi2::integer_attribute(
        to_bounded_scalar_variable_attributes<int, fmuproxy::thrift::IntegerAttribute>(a));
}

fmi4cpp::fmi2::real_attribute convert(const fmuproxy::thrift::RealAttribute& a)
{
    return fmi4cpp::fmi2::real_attribute(
        to_bounded_scalar_variable_attributes<double, fmuproxy::thrift::RealAttribute>(a));
}

fmi4cpp::fmi2::string_attribute convert(const fmuproxy::thrift::StringAttribute& a)
{
    return fmi4cpp::fmi2::string_attribute(
        to_scalar_variable_attributes<std::string, fmuproxy::thrift::StringAttribute>(a));
}

fmi4cpp::fmi2::boolean_attribute convert(const fmuproxy::thrift::BooleanAttribute& a)
{
    return fmi4cpp::fmi2::boolean_attribute(
        to_scalar_variable_attributes<bool, fmuproxy::thrift::BooleanAttribute>(a));
}

fmi4cpp::fmi2::enumeration_attribute convert(const fmuproxy::thrift::EnumerationAttribute& a)
{
    return fmi4cpp::fmi2::enumeration_attribute(
        to_bounded_scalar_variable_attributes<int, fmuproxy::thrift::EnumerationAttribute>(a));
}

fmi4cpp::fmi2::scalar_variable convert(const fmuproxy::thrift::ScalarVariable& v)
{

    fmi4cpp::fmi2::scalar_variable_base base;
    base.name = v.name;
    base.description = v.description;
    base.value_reference = (fmi2ValueReference)v.value_reference;
    base.variability = fmi4cpp::fmi2::parse_variability(v.variability);
    base.causality = fmi4cpp::fmi2::parse_causality(v.causality);
    base.initial = fmi4cpp::fmi2::parse_initial(v.initial);

    if (v.attribute.__isset.integer_attribute) {
        return {base, convert(v.attribute.integer_attribute)};
    } else if (v.attribute.__isset.real_attribute) {
        return {base, convert(v.attribute.real_attribute)};
    } else if (v.attribute.__isset.string_attribute) {
        return {base, convert(v.attribute.string_attribute)};
    } else if (v.attribute.__isset.boolean_attribute) {
        return {base, convert(v.attribute.boolean_attribute)};
    } else if (v.attribute.__isset.enumeration_attribute) {
        return {base, convert(v.attribute.enumeration_attribute)};
    } else {
        throw std::runtime_error("Fatal: no attribute set!");
    }
}

std::unique_ptr<fmi4cpp::fmi2::model_variables> convert(const fmuproxy::thrift::ModelVariables& m)
{
    std::vector<fmi4cpp::fmi2::scalar_variable> variables;
    for (const auto& var : m) {
        variables.push_back(convert(var));
    }
    return std::make_unique<fmi4cpp::fmi2::model_variables>(variables);
}

std::unique_ptr<fmi4cpp::fmi2::cs_model_description> convert(const fmuproxy::thrift::ModelDescription& from)
{
    fmi4cpp::fmi2::model_description_base base;
    base.guid = from.guid;
    base.fmi_version = from.fmi_version;
    base.description = from.description;
    base.model_name = from.modelName;
    base.model_variables = std::move(convert(from.model_variables));
    base.variable_naming_convention = from.variable_naming_convention;
    base.default_experiment = convert(from.default_experiment);
    base.generation_date_and_time = from.generation_date_and_time;
    base.generation_tool = from.generation_tool;
    base.license = from.license;
    base.version = from.version;
    base.copyright = from.copyright;
    base.author = from.author;
    base.model_structure = nullptr;

    fmi4cpp::fmi2::cs_attributes attributes;
    attributes.model_identifier = from.model_identifier;
    attributes.can_get_and_set_fmu_state = from.can_get_and_set_fmu_state;
    attributes.can_serialize_fmu_state = from.can_serialize_fmu_state;
    attributes.provides_directional_derivative = from.provides_directional_derivative;

    attributes.can_interpolate_inputs = from.can_interpolate_inputs;
    attributes.can_handle_variable_communication_step_size = from.can_handle_variable_communication_step_size;
    attributes.max_output_derivative_order = static_cast<unsigned int>(from.max_output_derivative_order);

    return std::make_unique<fmi4cpp::fmi2::cs_model_description>(base, attributes);
}

} // namespace

#endif //FMU_PROXY_THRIFT_CLIENT_HELPER_HPP
