
#ifndef FMU_PROXY_THRIFT_SERVER_HELPER_HPP
#define FMU_PROXY_THRIFT_SERVER_HELPER_HPP

#include <fmuproxy/fmi/model_description.hpp>
#include <fmuproxy/thrift/FmuService.h>

using namespace fmuproxy::thrift;

namespace fmuproxy::server
{

void make_model_description(ModelDescription& _return, const fmi::model_description& md)
{
    _return.guid = md.guid;
    _return.author = md.author;
    _return.model_name = md.model_name;
    _return.fmi_version = md.fmi_version;
    _return.description = md.description;
    _return.model_identifier = md.model_identifier;
    _return.generation_tool = md.generation_tool;
    _return.generation_date_and_time = md.generation_date_and_time;

    for (const auto& _v: md.model_variables) {
        ScalarVariable v;
        v.value_reference = _v.vr;
        v.name = _v.name;
        auto _attr = _v.typeAttribute;
        if (_v.is_real()) {
            RealAttribute r;
            auto _r = std::get<fmi::real>(_attr);
            if (_r.start) {
                r.__set_start(*_r.start);
            }
            v.attribute.__set_real_attribute(r);
        } else if (_v.is_integer()) {
            IntegerAttribute i;
            auto _i = std::get<fmi::integer>(_attr);
            if (_i.start) {
                i.__set_start(*_i.start);
            }
            v.attribute.__set_integer_attribute(i);
        } else if (_v.is_string()) {
            StringAttribute s;
            auto _s = std::get<fmi::string>(_attr);
            if (_s.start) {
                s.__set_start(*_s.start);
            }
            v.attribute.__set_string_attribute(s);
        } else if (_v.is_boolean()) {
            BooleanAttribute b;
            auto _b = std::get<fmi::boolean>(_attr);
            if (_b.start) {
                b.__set_start(*_b.start);
            }
            v.attribute.__set_boolean_attribute(b);
        }
        _return.model_variables.push_back(v);
    }
}

} // namespace


#endif //FMU_PROXY_THRIFT_SERVER_HELPER_HPP
