
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
#include <string>
#include <vector>

namespace proxyfmu
{

// State shared between start_process() and the thread that waits for it to
// report the port. It is held by both through a shared_ptr, as the thread
// which runs start_process() may outlive the waiting one.
struct process_state
{
    std::mutex mtx;
    std::condition_variable cv;
    int port = -1; // -999 if the process could not be started
    std::exception_ptr error; // what start_process() threw, if anything
};

void run_process(
    const proxyfmu::filesystem::path& fmuPath,
    const std::string& instanceName,
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

// Starts a proxy process for the FMU, and reports its port through `state`.
// Meant to run in a separate thread, so exceptions are passed on through
// `state` instead of ending the thread, which would terminate the program.
void start_process(
    const proxyfmu::filesystem::path& fmuPath,
    const std::string& instanceName,
    std::shared_ptr<process_state> state)
{
    try {
        run_process(fmuPath, instanceName, *state);
    } catch (...) {
        std::lock_guard<std::mutex> lck(state->mtx);
        state->error = std::current_exception();
        state->port = -999;
        state->cv.notify_one();
    }
}

} // namespace proxyfmu

#endif // PROXYFMU_PROCESS_HELPER_HPP
