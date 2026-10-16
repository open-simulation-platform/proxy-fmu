
#ifndef PROXYFMU_PROCESS_HELPER_HPP
#define PROXYFMU_PROCESS_HELPER_HPP

#include <proxyfmu/fs_portability.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/process.hpp>

#include <condition_variable>
#include <exception>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <string>

namespace proxyfmu
{

/**
 * Returns a unique path for a Unix domain socket in the temporary directory,
 * or an empty string if Unix domain sockets cannot be used, in which case TCP
 * should be used instead.
 *
 * The Thrift build used on Windows lacks AF_UNIX support, so Windows always
 * gets an empty string.
 */
inline std::string unix_socket_path()
{
#ifdef _WIN32
    return {};
#else
    std::random_device rd;
    std::ostringstream name;
    name << "proxyfmu_" << std::hex << rd() << rd() << ".sock";
    const auto path = (proxyfmu::filesystem::temp_directory_path() / name.str()).string();
    // sun_path holds 108 bytes on Linux and 104 on macOS, including the terminating null.
    constexpr std::size_t maxPathLength = 103;
    return path.size() <= maxPathLength ? path : std::string();
#endif
}

/**
 * Starts a proxyfmu server process for the given FMU and waits for it to exit.
 *
 * Once the server is ready, `port` is set to its TCP port or, if `socketPath`
 * is non-empty, to 0 to signal that it listens on that Unix domain socket.
 * It is set to -999 if the server could not be started.
 */
void start_process(
    const proxyfmu::filesystem::path& fmuPath,
    const std::string& instanceName,
    const std::string& socketPath,
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

    std::string cmd(execStr + " --fmu \"" + fmuPath.string() + "\" --instanceName \"" + instanceName + "\"");
    if (!socketPath.empty()) {
        cmd += " --unixSocket \"" + socketPath + "\"";
    }

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
        } else if (!bound && line.substr(0, 18) == "[proxyfmu] socket=") {
            {
                std::lock_guard<std::mutex> lck(mtx);
                port = 0;
                std::cout << "[proxyfmu] FMU instance '" << instanceName << "' instantiated using socket " << line.substr(18) << std::endl;
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
