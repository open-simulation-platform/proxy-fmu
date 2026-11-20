
#include "fmi1/fmi1_fmu.hpp"
#include "fmi2/fmi2_fmu.hpp"
#include <fmilib.h>
#include <proxyfmu/temp_dir.hpp>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <system_error>

#if defined(__APPLE__)
#    include <mach-o/fat.h>
#    include <mach-o/loader.h>
#elif defined(__linux__)
#    include <elf.h>
#endif

namespace proxyfmu::fmi
{

namespace
{

// FMI 2.0.4 only has platform names for x86 (e.g. "darwin64" and "linux64"),
// and uses the FMI 3 platform tuple (e.g. "aarch64-darwin") for other
// platforms. Returns the x86 name that FMUs sometimes use for the latter
// anyway, or an empty string.
constexpr std::string_view x86_platform_name(std::string_view platform)
{
    if (platform == "aarch64-darwin") return "darwin64";
    if (platform == "aarch64-linux") return "linux64";
    return {};
}

#if defined(__APPLE__) || defined(__linux__)
// Returns whether the shared library at `path` contains 64-bit ARM code.
// On macOS, this may be a universal library with an arm64 slice.
bool contains_aarch64_code(const filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);
    const auto read_uint32 = [&file](bool bigEndian) {
        unsigned char b[4] = {};
        file.read(reinterpret_cast<char*>(b), sizeof b);
        return bigEndian
            ? std::uint32_t{b[0]} << 24 | std::uint32_t{b[1]} << 16 | std::uint32_t{b[2]} << 8 | b[3]
            : std::uint32_t{b[3]} << 24 | std::uint32_t{b[2]} << 16 | std::uint32_t{b[1]} << 8 | b[0];
    };
#    if defined(__APPLE__)
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
#    else
    unsigned char ident[EI_NIDENT] = {};
    file.read(reinterpret_cast<char*>(ident), sizeof ident);
    if (!file || std::memcmp(ident, ELFMAG, SELFMAG) != 0 ||
        ident[EI_CLASS] != ELFCLASS64 || ident[EI_DATA] != ELFDATA2LSB) {
        return false;
    }
    // e_type and e_machine follow e_ident, both 16 bits.
    const auto typeAndMachine = read_uint32(false);
    return !!file && (typeAndMachine >> 16) == EM_AARCH64;
#    endif
}
#endif

// FMI 2.0.4 reserves names like "darwin64" and "linux64" for 64-bit x86, so
// e.g. FMI 2 FMUs keep their Apple silicon binaries in "aarch64-darwin",
// which is where FMI Library looks for them on such machines. Many FMUs only
// have a universal (or even an arm64) library in "darwin64", though, so if
// the platform folder is missing and the x86 folder contains code for this
// platform, we make the former a link to the latter in the unpacked FMU.
void link_x86_platform_folder_if_platform_folder_missing(
    [[maybe_unused]] const filesystem::path& fmuPath,
    [[maybe_unused]] const filesystem::path& fmuUnpackDir,
    [[maybe_unused]] const std::string& modelIdentifier)
{
#if defined(__APPLE__) || defined(__linux__)
    constexpr auto platform = std::string_view(FMI2_PLATFORM);
    constexpr auto x86Platform = x86_platform_name(platform);
    if (x86Platform.empty()) return;
#    if defined(__APPLE__)
    constexpr auto librarySuffix = ".dylib";
#    else
    constexpr auto librarySuffix = ".so";
#    endif

    const auto binariesDir = fmuUnpackDir / "binaries";
    const auto platformDir = binariesDir / platform;
    if (filesystem::exists(filesystem::symlink_status(platformDir))) return;

    const auto library = binariesDir / x86Platform / (modelIdentifier + librarySuffix);
    if (!filesystem::exists(library)) return;
    if (!contains_aarch64_code(library)) {
        std::cerr << "[proxyfmu] Warning: FMU '" << fmuPath.string()
                  << "' has no binaries/" << platform << ", and its " << x86Platform
                  << " library contains no code for this platform" << std::endl;
        return;
    }
    std::error_code linkError;
    filesystem::create_directory_symlink(x86Platform, platformDir, linkError);
    if (linkError) {
        std::cerr << "[proxyfmu] Warning: FMU '" << fmuPath.string()
                  << "' has no binaries/" << platform << ", and linking it to " << x86Platform
                  << " failed: " << linkError.message() << std::endl;
        return;
    }
    std::cerr << "[proxyfmu] Warning: FMU '" << fmuPath.string()
              << "' has no binaries/" << platform << "; using " << x86Platform
              << ", which FMI 2.0.4 reserves for x86" << std::endl;
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
        link_x86_platform_folder_if_platform_folder_missing(
            fmuPath, tmp->path(), fmu->get_model_description().modelIdentifier);
        return fmu;
    } else {
        // TODO
    }

    return nullptr;
}

} // namespace proxyfmu::fmi