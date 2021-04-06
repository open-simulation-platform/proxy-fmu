
#include "../util/temp_dir.hpp"

#include "fmi1/fmi1_fmu.hpp"
#include "fmi2/fmi2_fmu.hpp"
#include <fmilib.h>

namespace proxyfmu::fmi
{

std::unique_ptr<fmu> loadFmu(const filesystem::path& fmuPath)
{

    auto ctx = std::make_unique<fmicontext>();

    const std::string fmuName = proxyfmu::filesystem::path(fmuPath).stem().string();
    auto tmp = std::make_shared<util::temp_dir>(fmuName);

    fmi_version_enu_t version = fmi_import_get_fmi_version(ctx->ctx_, fmuPath.string().c_str(), tmp->path().string().c_str());
    if (version == fmi_version_1_enu) {
        return std::make_unique<fmi1_fmu>(std::move(ctx), tmp);
    } else if (version == fmi_version_2_0_enu) {
        return std::make_unique<fmi2_fmu>(std::move(ctx), tmp);
    } else {
        //TODO
    }

    return nullptr;
}

} // namespace proxyfmu::fmi