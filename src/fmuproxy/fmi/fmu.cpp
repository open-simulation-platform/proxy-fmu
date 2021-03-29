
#include "../util/temp_dir.hpp"

#include "fmi1/fmi1_fmu.hpp"
#include "fmi2/fmi2_fmu.hpp"
#include <fmilib.h>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace fmuproxy::fmi
{

std::unique_ptr<fmu> loadFmu(const std::filesystem::path& fmuPath)
{

    auto ctx = std::make_unique<fmicontext>();

    const std::string fmuName = fs::path(fmuPath).stem().string();
    auto tmp = std::make_shared<fmuproxy::util::temp_dir>(fmuName);

    fmi_version_enu_t version = fmi_import_get_fmi_version(ctx->ctx_, fmuPath.string().c_str(), tmp->path().string().c_str());
    if (version == fmi_version_1_enu) {
        std::cout << "FMI version 1.0 detected" << std::endl;
        return std::make_unique<fmi1_fmu>(std::move(ctx), tmp);
    } else if (version == fmi_version_2_0_enu) {
        std::cout << "FMI version 2.0 detected" << std::endl;
        return std::make_unique<fmi2_fmu>(std::move(ctx), tmp);
    } else {
        //TODO
    }

    return nullptr;
}

} // namespace fmuproxy::fmi