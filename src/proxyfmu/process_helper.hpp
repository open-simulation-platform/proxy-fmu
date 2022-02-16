
#ifndef PROXYFMU_PROCESS_HELPER_HPP
#define PROXYFMU_PROCESS_HELPER_HPP

#include <proxyfmu/fs_portability.hpp>

#include <condition_variable>
#include <exception>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <subprocess/subprocess.h>


#ifdef WIN32
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#endif
#ifdef __linux__
#    include <algorithm>
#    include <unistd.h>
#endif

namespace proxyfmu
{

std::optional<std::string> getLoc()
{
    char pBuf[256];
    size_t len = sizeof(pBuf);
#ifdef __linux__
    int bytes = std::min(static_cast<int>(readlink("/proc/self/exe", pBuf, len)), static_cast<int>(len - 1));
    if (bytes >= 0) {
        pBuf[bytes] = '\0';
        return {pBuf};
    }
#else
    int bytes = GetModuleFileName(nullptr, pBuf, len);
    if (bytes) return {pBuf};
#endif
    return std::nullopt;
}

void start_process(
    const proxyfmu::filesystem::path& fmuPath,
    const std::string& instanceName,
    int& port,
    std::mutex& mtx,
    std::condition_variable& cv)
{

    proxyfmu::filesystem::path executable;
#ifdef __linux__
    executable = "proxyfmu";
#else
    executable = "proxyfmu.exe";
#endif

    if (!proxyfmu::filesystem::exists(executable)) {
        auto loc = getLoc();
        if (loc) {
            executable = proxyfmu::filesystem::path(*loc).parent_path().string() / executable;
        } else {
            std::cerr << "[proxyfmu] Error, unable to locate parent executable" << std::endl;
        }
    }

    if (!proxyfmu::filesystem::exists(executable)) {
        auto execPath = proxyfmu::filesystem::absolute(executable).string();
        throw std::runtime_error("[proxyfmu] No proxyfmu executable found. " + execPath + " does not exist!");
    }

    std::cout << "[proxyfmu] Found proxyfmu executable: " << executable << std::endl;
    std::cout << "[proxyfmu] Booting FMU instance '" << instanceName << "'.." << std::endl;

    std::string executableStr = executable.string();
    std::string fmuPathStr = fmuPath.string();
    std::vector<const char*> cmd = {executableStr.c_str(), fmuPathStr.c_str(), instanceName.c_str(), nullptr};

#ifdef __linux__
    if (!executable.is_absolute()) {
        cmd.insert(cmd.begin(), "./");
    }
#endif

    struct subprocess_s process;
    int result = subprocess_create(cmd.data(), subprocess_option_inherit_environment | subprocess_option_no_window, &process);

    bool bound = false;
    if (result == 0) {
        FILE* p_stdout = subprocess_stdout(&process);

        char buffer[256];
        while (fgets(buffer, 256, p_stdout)) {
            std::string line(buffer);
            if (!bound && line.substr(0, 16) == "[proxyfmu] port=") {
                {
                    std::lock_guard<std::mutex> lck(mtx);
                    port = std::stoi(line.substr(16));
                    std::cout << "[proxyfmu] FMU instance '" << instanceName << "' instantiated using port " << port << std::endl;
                }
                cv.notify_one();
                bound = true;
            } else if (line.substr(0, 16) == "[proxyfmu] freed") {
                break;
            } else {
                std::cerr << line << std::endl;
            }
        }
    }

    int status = -999;
    result = subprocess_join(&process, &status);
    if (result == 0) {
        std::cout << "[proxyfmu] External proxy process for instance '" << instanceName << "' returned with status " << std::to_string(status) << std::endl;
    }

    // exit code -999 has special meaning: not able to bind to a port
    if (!bound && status == -999) {
        {
            std::lock_guard<std::mutex> lck(mtx);
            std::cerr << "[proxyfmu] Unable to bind to external proxy process!" << std::endl;
            port = -999;
        }
        cv.notify_one();
    }
}

} // namespace proxyfmu

#endif // PROXYFMU_PROCESS_HELPER_HPP
