
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
#include <vector>


#ifdef WIN32
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#endif
#ifdef __linux__
#    include <climits>
#    include <cstring>
#    include <unistd.h>
#endif

namespace proxyfmu
{

std::string getLoc()
{

#ifdef __linux__
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return std::string(result, (count > 0) ? count : 0);
#else
    char result[MAX_PATH];
    return std::string(result, GetModuleFileName(nullptr, result, MAX_PATH));
#endif
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
        executable = proxyfmu::filesystem::path(loc).parent_path().string() / executable;
    }

    if (!proxyfmu::filesystem::exists(executable)) {
        auto execPath = proxyfmu::filesystem::absolute(executable).string();
        throw std::runtime_error("[proxyfmu] No proxyfmu executable found. " + execPath + " does not exist!");
    }

    std::cout << "[proxyfmu] Found proxyfmu executable: " << executable << " version ";
    std::cout.flush();

    std::string executableStr = executable.string();
#ifdef __linux__
    executableStr.insert(0, "./");
#endif

    system((executableStr + " -v").c_str());
    std::cout << "\n";
    std::cout << "[proxyfmu] Booting FMU instance '" << instanceName
              << "'. Source: '" << proxyfmu::filesystem::absolute(fmuPath) << "'" << std::endl;

    std::string fmuPathStr = fmuPath.string();
    std::vector<const char*> cmd = {executableStr.c_str(), fmuPathStr.c_str(), instanceName.c_str(), nullptr};

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
