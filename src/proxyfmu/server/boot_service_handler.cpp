
#include <proxyfmu/server/boot_service_handler.hpp>

#include <cstdio>
#include <string>

using namespace proxyfmu::server;

namespace {

void write_data(std::string const& fileName, std::string const& data)
{
    FILE* file = fopen(fileName.c_str(), "wb");
    [[maybe_unused]] size_t bytes_written = fwrite(data.c_str(), sizeof(unsigned char), data.size(), file);
    fclose(file);
}

void start_process(const proxyfmu::filesystem::path& fmu, const std::string& instanceName, const int port)
{
    std::string cmd(
        "proxy_server"
        " --port " +
        std::to_string(port) +
        " --fmu \"" + fmu.string() + +"\""
                                      " --instanceName " +
        instanceName);
    auto status = system(cmd.c_str());
    std::cout << "External proxy process returned with status " << std::to_string(status) << std::endl;
}

}

int64_t boot_service_handler::loadFromBinaryData(const std::string& fmuName, const std::string& instanceName, const std::string& data)
{
    auto tmp = std::make_unique<temp_dir>(fmuName);
    std::string fmuPath(tmp->path().string() + "/" + fmuName  + ".fmu");

    write_data(fmuPath, data);

    auto port = 9090;
    auto t = std::make_unique<std::thread>(&start_process, fmuPath, instanceName, port);

    dirs_.emplace_back(std::move(tmp));

    return port;
}
