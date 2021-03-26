
#include "fmi1_model_description.hpp"

namespace fmi
{

fmi1_model_description::fmi1_model_description(fmi1_import_t* handle)
{
    guid_ = fmi1_import_get_GUID(handle);
    author_ = fmi1_import_get_author(handle);
    modelName_ = fmi1_import_get_model_name(handle);
}

std::string fmi1_model_description::get_guid()
{
    return guid_;
}

std::string fmi1_model_description::get_author()
{
    return author_;
}

std::string fmi1_model_description::get_model_name()
{
    return modelName_;
}

std::string fmi1_model_description::get_fmi_version()
{
    return fmiVersion_;
}

std::string fmi1_model_description::get_description()
{
    return description_;
}

std::string fmi1_model_description::get_generation_tool()
{
    return generationTool_;
}

std::string fmi1_model_description::get_generation_date_and_time()
{
    return generationDateAndTime_;
}

std::optional<default_experiment> fmi1_model_description::get_default_experiment()
{
    return std::optional<default_experiment>();
}

model_variables fmi1_model_description::get_model_variables()
{
    return modelVariables_;
}


} // namespace fmi