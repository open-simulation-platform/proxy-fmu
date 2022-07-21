
#ifndef PROXY_FMU_FMI1_MODEL_DESCRIPTION_HPP
#define PROXY_FMU_FMI1_MODEL_DESCRIPTION_HPP

#include <fmilib.h>
#include <proxyfmu/fmi/model_description.hpp>

namespace proxyfmu::fmi
{

model_description create_model_description(fmi1_import_t* handle);

} // namespace proxyfmu::fmi

#endif // PROXY_FMU_FMI1_MODEL_DESCRIPTION_HPP
