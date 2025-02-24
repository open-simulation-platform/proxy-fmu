
#ifndef PROXY_FMU_MODEL_DESCRIPTION_HPP
#define PROXY_FMU_MODEL_DESCRIPTION_HPP

#include <proxyfmu/fmi/scalar_variable.hpp>

#include <optional>
#include <vector>

namespace proxyfmu::fmi
{

struct default_experiment
{
    double startTime;
    double stopTime;
    double tolerance;
    double stepSize;
};

struct model_description
{
    std::string guid;
    std::string author;
    std::string modelName;
    std::string modelIdentifier;
    std::string fmiVersion;
    std::string description;
    std::string generationTool;
    std::string generationDateAndTime;

    model_variables modelVariables;
    default_experiment defaultExperiment;

    bool canGetAndSetFMUstate;
    bool canSerializeFMUstate;
};

} // namespace proxyfmu::fmi

#endif // PROXY_FMU_MODEL_DESCRIPTION_HPP
