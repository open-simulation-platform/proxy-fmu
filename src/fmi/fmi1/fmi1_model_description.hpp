
#ifndef FMU_PROXY_FMI1_MODEL_DESCRIPTION_HPP
#define FMU_PROXY_FMI1_MODEL_DESCRIPTION_HPP

#include <fmi/model_description.hpp>
#include <fmilib.h>

namespace fmi
{

class fmi1_model_description : public model_description
{
private:
    std::string guid_;
    std::string author_;
    std::string modelName_;
    std::string description_;
    std::string generationTool_;
    std::string generationDateAndTime_;
    std::string fmiVersion_ = "1.0";

    model_variables modelVariables_;

public:

    explicit fmi1_model_description(fmi1_import_t* handle);

    std::string get_guid() override;
    std::string get_author() override;
    std::string get_model_name() override;
    std::string get_fmi_version() override;
    std::string get_description() override;
    std::string get_generation_tool() override;
    std::string get_generation_date_and_time() override;
    std::optional<default_experiment> get_default_experiment() override;
    fmi::model_variables get_model_variables() override;
};

} // namespace fmi

#endif //FMU_PROXY_FMI1_MODEL_DESCRIPTION_HPP
