
#ifndef PROXYFMU_PROCESS_HELPER_HPP
#define PROXYFMU_PROCESS_HELPER_HPP

#include <proxyfmu/fs_portability.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/process.hpp>

#include <algorithm>
#include <condition_variable>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <vector>

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

// State shared between start_process() and the thread that waits for it to
// report the port. It is held by both through a shared_ptr, as the thread
// which runs start_process() may outlive the waiting one.
struct process_state
{
    std::mutex mtx;
    std::condition_variable cv;
    int port = -1; // 0 if it listens on a Unix domain socket, -999 if it could not be started
    std::exception_ptr error; // what start_process() threw, if anything
};

/**
 * Starts a proxyfmu server process for the given FMU and waits for it to exit.
 *
 * Once the server is ready, `state.port` is set to its TCP port or, if
 * `socketPath` is non-empty, to 0 to signal that it listens on that Unix
 * domain socket. It is set to -999 if the server could not be started.
 */
void run_process(
    const proxyfmu::filesystem::path& fmuPath,
    const std::string& instanceName,
    const std::string& socketPath,
    process_state& state)
{

    proxyfmu::filesystem::path executable;
#ifdef _WIN32
    executable = "proxyfmu.exe";
#else
    executable = "proxyfmu";
#endif

    // Where to look for the executable, in this order:
    //  1. the current directory;
    //  2. the directory of this library (proxyfmu-client, or the module it is
    //     linked into), and its ../bin (an installed lib/ + bin/ layout): this
    //     finds an executable shipped with a library that embeds proxyfmu,
    //     whatever program loads it (e.g. a Python or Java interpreter);
    //  3. the directory of the running program.
    std::vector<proxyfmu::filesystem::path> candidates{executable};
    {
        boost::dll::fs::error_code ec;
        const auto library = boost::dll::symbol_location_ptr(reinterpret_cast<const void*>(&run_process), ec);
        if (!ec) {
            const auto dir = proxyfmu::filesystem::path(library.parent_path().string());
            candidates.push_back(dir / executable);
            candidates.push_back(dir.parent_path() / "bin" / executable);
        }
    }
    {
        boost::dll::fs::error_code ec;
        const auto program = boost::dll::program_location(ec);
        if (!ec) {
            candidates.push_back(proxyfmu::filesystem::path(program.parent_path().string()) / executable);
        } else {
            std::cerr << "[proxyfmu] Error, unable to locate parent executable" << std::endl;
        }
    }
    const auto found = std::find_if(candidates.begin(), candidates.end(), [](const auto& candidate) {
        return proxyfmu::filesystem::exists(candidate);
    });
    if (found == candidates.end()) {
        std::string searched;
        for (const auto& candidate : candidates) {
            searched += (searched.empty() ? "" : ", ") + proxyfmu::filesystem::absolute(candidate).string();
        }
        throw std::runtime_error("[proxyfmu] No proxyfmu executable found. Searched: " + searched);
    }
    executable = *found;

    std::string execStr = executable.string();
#ifndef _WIN32
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
                std::lock_guard<std::mutex> lck(state.mtx);
                state.port = std::stoi(line.substr(16));
                std::cout << "[proxyfmu] FMU instance '" << instanceName << "' instantiated using port " << state.port << std::endl;
            }
            state.cv.notify_one();
            bound = true;
        } else if (!bound && line.substr(0, 18) == "[proxyfmu] socket=") {
            {
                std::lock_guard<std::mutex> lck(state.mtx);
                state.port = 0;
                std::cout << "[proxyfmu] FMU instance '" << instanceName << "' instantiated using socket " << line.substr(18) << std::endl;
            }
            state.cv.notify_one();
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
        std::lock_guard<std::mutex> lck(state.mtx);
        state.port = -999;

        state.cv.notify_one();
    }
}

// Starts a proxy process for the FMU, and reports its port through `state`
// (see run_process()). Meant to run in a separate thread, so exceptions are
// passed on through `state` instead of ending the thread, which would
// terminate the program.
void start_process(
    const proxyfmu::filesystem::path& fmuPath,
    const std::string& instanceName,
    const std::string& socketPath,
    std::shared_ptr<process_state> state)
{
    try {
        run_process(fmuPath, instanceName, socketPath, *state);
    } catch (...) {
        std::lock_guard<std::mutex> lck(state->mtx);
        state->error = std::current_exception();
        state->port = -999;
        state->cv.notify_one();
    }
}

} // namespace proxyfmu

#endif // PROXYFMU_PROCESS_HELPER_HPP
