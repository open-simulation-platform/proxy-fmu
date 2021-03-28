
#ifndef FMU_PROXY_FMI1_MODEL_DESCRIPTION_HPP
#define FMU_PROXY_FMI1_MODEL_DESCRIPTION_HPP

#include <fmi/model_description.hpp>
#include <fmilib.h>

namespace fmi
{

model_description create_model_description(fmi1_import_t* handle);

} // namespace fmi

#endif //FMU_PROXY_FMI1_MODEL_DESCRIPTION_HPP
