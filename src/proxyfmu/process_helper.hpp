
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

    std::string cmd("proxyfmu --fmu " + fmuPath.string() + " --instanceName " + instanceName);

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

#ifdef __linux__
    if (!executable.is_absolute()) {
        cmd.insert(0, "./");
    }
#endif

    if (!proxyfmu::filesystem::exists(executable)) {
        auto execPath = proxyfmu::filesystem::absolute(executable).string();
        throw std::runtime_error("[proxyfmu] No proxyfmu executable found. " + execPath + " does not exist!");
    }

    std::cout << "[proxyfmu] executable: " << executable << std::endl;
    std::cout << "[proxyfmu] Booting FMU instance '" << instanceName << "'.." << std::endl;

    std::cout << "cmd=" << cmd << std::endl;

    boost::process::ipstream pipe_stream;
    boost::process::child c(cmd, boost::process::std_out > pipe_stream);

    std::string line;
    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty()) {
        if (line.substr(0, 16) == "[proxyfmu] port=") {
            port = std::stoi(line.substr(16));
            std::unique_lock<std::mutex> lck(mtx);
            cv.notify_all();
            std::cout << "[proxyfmu] FMU instance '" << instanceName << "' instantiated using port " << port << std::endl;
        } else if (line.substr(0, 10) == "[proxyfmu]") {
            std::cout << line << std::endl;
        }
    }

    c.wait();

    auto status = c.exit_code();
    std::cout << "[proxyfmu] External proxy process for instance '" << instanceName << "' returned with status " << std::to_string(status) << std::endl;

    if (status != 0) {
        std::cerr << "[proxyfmu] Unable to bind to external proxy process!" << std::endl;
        throw std::runtime_error("[proxyfmu] Unable to bind to external proxy process!");
    }
}

} // namespace proxyfmu

#endif // PROXYFMU_PROCESS_HELPER_HPP
