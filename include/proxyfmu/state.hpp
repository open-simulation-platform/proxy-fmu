#ifndef PROXYFMU_STATE_HPP
#define PROXYFMU_STATE_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace proxyfmu::state
{
constexpr std::int32_t export_scheme_version = 0;

using state_index = int;
using fmi2_FMU_state_t = void*;

struct saved_state
{
    fmi2_FMU_state_t fmuState = nullptr;
    bool setupComplete = false;
    bool simStarted = false;
};

struct exported_state
{
    std::int32_t schemeVersion;
    std::string uuid;
    std::string fmuState;
    bool setupComplete;
    bool simStarted;
};

}


#endif // PROXYFMU_STATE_HPP
