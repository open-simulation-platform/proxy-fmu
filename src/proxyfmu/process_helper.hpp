
#ifndef PROXYFMU_PROCESS_HELPER_HPP
#define PROXYFMU_PROCESS_HELPER_HPP

#include <proxyfmu/fs_portability.hpp>

#include <boost/process.hpp>

#include <iostream>

namespace proxyfmu
{

void start_process(const proxyfmu::filesystem::path& fmuPath, const std::string& instanceName, int& port)
{
    std::string cmd("proxyfmu --fmu " + fmuPath.string() + " --instanceName " + instanceName);

#ifdef __linux__
    cmd.insert(0, "./");
#endif

    boost::process::ipstream pipe_stream;
    boost::process::child c(cmd, boost::process::std_out > pipe_stream);

    std::string line;
    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty()) {
        if (line.substr(0, 16) == "[proxyfmu] port=") {
            port = std::stoi(line.substr(16));
        }
    }

    c.wait();

    auto status = c.exit_code();
    std::cout << "[proxyfmu] External proxy process for instance '" << instanceName << "' returned with status " << std::to_string(status) << std::endl;
}

} // namespace proxyfmu

#endif //PROXYFMU_PROCESS_HELPER_HPP
