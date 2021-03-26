
#include "temp_dir.hpp"

#include "fmi1/fmi1_fmu.hpp"
#include "fmi2/fmi2_fmu.hpp"
#include <fmilib.h>
#include "uuid.hpp"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace
{

void logger_callback(
    jm_callbacks* /*callbacks*/,
    jm_string module,
    jm_log_level_enu_t logLevel,
    jm_string message)
{
    printf("module = %s, log level = %d: %s\n", module, logLevel, message);
}

void fmilogger(fmi2_component_t c, fmi2_string_t instanceName, fmi2_status_t status, fmi2_string_t category, fmi2_string_t message, ...)
{

}

} // namespace

namespace fmi
{

std::unique_ptr<fmu> loadFmu(const std::filesystem::path& fmuPath)
{
    jm_callbacks callbacks;
    callbacks.malloc = std::malloc;
    callbacks.calloc = std::calloc;
    callbacks.realloc = std::realloc;
    callbacks.free = std::free;
    callbacks.logger = logger_callback;
    callbacks.log_level = jm_log_level_error;
    callbacks.context = nullptr;

    fmi_import_context_t* ctx = fmi_import_allocate_context(&callbacks);

    const std::string fmuName = fs::path(fmuPath).stem().string();
    auto tmpPath(std::filesystem::temp_directory_path() /= "fmu_proxy_" + fmuName + "_" + generate_uuid());
    std::filesystem::create_directories(tmpPath);
    auto tmp = std::make_shared<temp_dir>(tmpPath);

    fmi_version_enu_t version = fmi_import_get_fmi_version(ctx, fmuPath.string().c_str(), tmp->path().string().c_str());
    if (version == fmi_version_1_enu) {
        std::cout << "FMI version 1.0 detected" << std::endl;
        return std::make_unique<fmi1_fmu>(ctx, tmp);
    } else if (version == fmi_version_2_0_enu) {
        std::cout << "FMI version 2.0 detected" << std::endl;
        return std::make_unique<fmi2_fmu>(ctx, tmp);
    } else {
        //TODO
    }

    return nullptr;
}

} // namespace fmi