
#ifndef FMU_PROXY_FMI1_MODEL_DESCRIPTION_HPP
#define FMU_PROXY_FMI1_MODEL_DESCRIPTION_HPP

#include <fmilib.h>
#include <fmuproxy/fmi/model_description.hpp>

namespace fmuproxy::fmi
{

model_description create_model_description(fmi1_import_t* handle);

} // namespace fmuproxy::fmi

#endif //FMU_PROXY_FMI1_MODEL_DESCRIPTION_HPP
