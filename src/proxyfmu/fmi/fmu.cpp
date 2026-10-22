
#include "fmi1/fmi1_fmu.hpp"
#include "fmi2/fmi2_fmu.hpp"
#include <fmilib.h>
#include <proxyfmu/temp_dir.hpp>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <system_error>

#ifdef __APPLE__
#    include <mach-o/fat.h>
#    include <mach-o/loader.h>
#endif

namespace proxyfmu::fmi
{

namespace
{

#ifdef __APPLE__
// Returns whether the Mach-O file at `path` contains arm64 code, i.e., is
// either an arm64 library or a universal library with an arm64 slice.
bool contains_arm64_code(const filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);
    const auto read_uint32 = [&file](bool bigEndian) {
        unsigned char b[4] = {};
        file.read(reinterpret_cast<char*>(b), sizeof b);
        return bigEndian
            ? std::uint32_t{b[0]} << 24 | std::uint32_t{b[1]} << 16 | std::uint32_t{b[2]} << 8 | b[3]
            : std::uint32_t{b[3]} << 24 | std::uint32_t{b[2]} << 16 | std::uint32_t{b[1]} << 8 | b[0];
    };

    const auto magic = read_uint32(true);
    if (magic == FAT_MAGIC || magic == FAT_MAGIC_64) {
        // Universal binary; the headers are big-endian.
        const auto archCount = read_uint32(true);
        const auto archSize = magic == FAT_MAGIC ? sizeof(fat_arch) : sizeof(fat_arch_64);
        for (std::uint32_t i = 0; file && i < archCount; ++i) {
            const auto archStart = file.tellg();
            if (read_uint32(true) == static_cast<std::uint32_t>(CPU_TYPE_ARM64)) return true;
            file.seekg(archStart + static_cast<std::streamoff>(archSize));
        }
        return false;
    }
    if (magic == MH_CIGAM_64) {
        // Thin 64-bit binary, little-endian.
        return read_uint32(false) == static_cast<std::uint32_t>(CPU_TYPE_ARM64) && !!file;
    }
    return false;
}
#endif

// FMI 2.0.4 reserves "darwin64" for 64-bit x86, so FMI 2 FMUs keep their
// Apple silicon binaries in "aarch64-darwin", which is where FMI Library
// looks for them on such machines. Many FMUs only have a universal (or even
// arm64) library in "darwin64", though, so if "aarch64-darwin" is missing
// and "darwin64" contains arm64 code, we make "aarch64-darwin" a link to
// "darwin64" in the unpacked FMU.
void link_darwin64_if_aarch64_darwin_missing(
    [[maybe_unused]] const filesystem::path& fmuPath,
    [[maybe_unused]] const filesystem::path& fmuUnpackDir,
    [[maybe_unused]] const std::string& modelIdentifier)
{
#ifdef __APPLE__
    constexpr auto platform = std::string_view(FMI2_PLATFORM);
    if (platform != "aarch64-darwin") return;

    const auto binariesDir = fmuUnpackDir / "binaries";
    const auto platformDir = binariesDir / platform;
    if (filesystem::exists(filesystem::symlink_status(platformDir))) return;

    const auto library = binariesDir / "darwin64" / (modelIdentifier + ".dylib");
    if (!filesystem::exists(library)) return;
    if (!contains_arm64_code(library)) {
        std::cerr << "[proxyfmu] Warning: FMU '" << fmuPath.string()
                  << "' has no binaries/aarch64-darwin, and its darwin64 library contains no arm64 code"
                  << std::endl;
        return;
    }
    std::error_code linkError;
    filesystem::create_directory_symlink("darwin64", platformDir, linkError);
    if (linkError) {
        std::cerr << "[proxyfmu] Warning: FMU '" << fmuPath.string()
                  << "' has no binaries/aarch64-darwin, and linking it to darwin64 failed: "
                  << linkError.message() << std::endl;
        return;
    }
    std::cerr << "[proxyfmu] Warning: FMU '" << fmuPath.string()
              << "' has no binaries/aarch64-darwin; using darwin64, which FMI 2.0.4 reserves for x86_64"
              << std::endl;
#endif
}

} // namespace

std::unique_ptr<fmu> loadFmu(const filesystem::path& fmuPath)
{
    auto ctx = std::make_unique<fmicontext>();

    const std::string fmuName = proxyfmu::filesystem::path(fmuPath).stem().string();
    auto tmp = std::make_shared<temp_dir>(fmuName);

    fmi_version_enu_t version = fmi_import_get_fmi_version(ctx->ctx_, fmuPath.string().c_str(), tmp->path().string().c_str());
    if (version == fmi_version_1_enu) {
        return std::make_unique<fmi1_fmu>(std::move(ctx), tmp);
    } else if (version == fmi_version_2_0_enu) {
        auto fmu = std::make_unique<fmi2_fmu>(std::move(ctx), tmp);
        link_darwin64_if_aarch64_darwin_missing(
            fmuPath, tmp->path(), fmu->get_model_description().modelIdentifier);
        return fmu;
    } else {
        // TODO
    }

    return nullptr;
}

} // namespace proxyfmu::fmi