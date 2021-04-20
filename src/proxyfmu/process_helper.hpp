
#ifndef PROXYFMU_PROCESS_HELPER_HPP
#define PROXYFMU_PROCESS_HELPER_HPP

#include <proxyfmu/fs_portability.hpp>

#include <iostream>

namespace proxyfmu
{

void start_process(const proxyfmu::filesystem::path& fmuPath, const std::string& instanceName, const int port)
{
    std::string cmd(
        "proxyfmu"
        " --port " +
        std::to_string(port) +
        " --fmu \"" + fmuPath.string() + +"\""
                                          " --instanceName " +
        instanceName);

#ifdef __linux__
    cmd.insert(0, "./")
#endif

    auto status = system(cmd.c_str());
    std::cout << "[proxyfmu] External proxy process for instance '" << instanceName << "' returned with status " << std::to_string(status) << std::endl;
}

} // namespace proxyfmu

#endif //PROXYFMU_PROCESS_HELPER_HPP
