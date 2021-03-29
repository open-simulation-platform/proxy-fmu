
#ifndef FMU_PROXY_FMI2_MODEL_DESCRIPTION_HPP
#define FMU_PROXY_FMI2_MODEL_DESCRIPTION_HPP

#include <fmuproxy/fmi/model_description.hpp>
#include <fmilib.h>

namespace fmuproxy::fmi
{

model_description create_model_description(fmi2_import_t* handle);

} // namespace fmi

#endif //FMU_PROXY_FMI2_MODEL_DESCRIPTION_HPP
