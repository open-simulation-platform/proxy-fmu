
#ifndef PROXYFMU_PROCESS_HELPER_HPP
#define PROXYFMU_PROCESS_HELPER_HPP

#include <proxyfmu/fs_portability.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/process.hpp>

#include <condition_variable>
#include <exception>
#include <iostream>
#include <mutex>
#include <string>

namespace proxyfmu
{

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
        boost::dll::fs::error_code ec;
        boost::dll::fs::path loc = boost::dll::program_location(ec);
        if (!ec.failed()) {
            executable = loc.parent_path().string() / executable;
        } else {
            std::cerr << "[proxyfmu] Error, unable to locate parent executable" << std::endl;
        }
    }

    if (!proxyfmu::filesystem::exists(executable)) {
        auto execPath = proxyfmu::filesystem::absolute(executable).string();
        throw std::runtime_error("[proxyfmu] No proxyfmu executable found. " + execPath + " does not exist!");
    }

    std::string execStr = executable.string();
#ifdef __linux__
    if (!executable.is_absolute()) {
        execStr.insert(0, "./");
    }
#endif

    std::cout << "[proxyfmu] Found proxyfmu executable: " << executable << " version: ";
    std::cout.flush();
    system((execStr + " -v").c_str());
    std::cout << "\n";
    std::cout << "[proxyfmu] Booting FMU instance '" << instanceName << "'.." << std::endl;

    std::string cmd(execStr + " --fmu \"" + fmuPath.string() + "\" --instanceName " + instanceName);

    boost::process::ipstream pipe_stream;
    boost::process::child c(cmd, boost::process::std_out > pipe_stream);

    bool bound = false;
    std::string line;
    while (pipe_stream && std::getline(pipe_stream, line)) {
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

    c.wait();
    int status = c.exit_code();

    if (status == 0 && bound) {
        return;
    } else {
        std::cerr << "[proxyfmu] External proxy process for instance '"
                  << instanceName << "' returned with status "
                  << std::to_string(status) << ". Unable to bind.." << std::endl;
        std::lock_guard<std::mutex> lck(mtx);
        port = -999;

        cv.notify_one();
    }
}

} // namespace proxyfmu

#endif // PROXYFMU_PROCESS_HELPER_HPP
