
#ifndef FMU_PROXY_CLIENT_HELPER_HPP
#define FMU_PROXY_CLIENT_HELPER_HPP

#include <fmuproxy/fmi/model_description.hpp>
#include <fmuproxy/thrift/FmuService.h>

using namespace fmuproxy::thrift;

namespace fmuproxy::client
{

void set_attribute(fmuproxy::fmi::scalar_variable& v, const ScalarVariableAttribute& attr)
{
    if (attr.__isset.real_attribute) {
        fmuproxy::fmi::real type;
        if (attr.real_attribute.__isset.start) {
            type.start = attr.real_attribute.start;
        }
        v.typeAttribute = type;
    } else if (attr.__isset.integer_attribute) {
        fmuproxy::fmi::integer type;
        if (attr.integer_attribute.__isset.start) {
            type.start = attr.integer_attribute.start;
        }
        v.typeAttribute = type;
    } else if (attr.__isset.string_attribute) {
        fmuproxy::fmi::string type;
        if (attr.string_attribute.__isset.start) {
            type.start = attr.string_attribute.start;
        }
        v.typeAttribute = type;
    } else if (attr.__isset.boolean_attribute) {
        fmuproxy::fmi::boolean type;
        if (attr.boolean_attribute.__isset.start) {
            type.start = attr.boolean_attribute.start;
        }
        v.typeAttribute = type;
    } else {
        //TODO
    }
}

void make_model_description(fmuproxy::fmi::model_description& _return, const ModelDescription& md)
{
    _return.guid = md.guid;
    _return.author = md.author;
    _return.model_name = md.model_name;
    _return.fmi_version = md.fmi_version;
    _return.description = md.description;
    _return.model_identifier = md.model_identifier;
    _return.generation_tool = md.generation_tool;
    _return.generation_date_and_time = md.generation_date_and_time;

    for (auto& _v : md.model_variables) {
        fmuproxy::fmi::scalar_variable v;
        v.name = _v.name;
        v.vr = _v.value_reference;

        set_attribute(v, _v.attribute);

        _return.model_variables.emplace_back(v);
    }
}

} // namespace fmuproxy::client

#endif //FMU_PROXY_CLIENT_HELPER_HPP
