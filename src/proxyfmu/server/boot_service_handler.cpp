
#include "../process_helper.hpp"

#include <proxyfmu/server/boot_service_handler.hpp>

#include <cstdio>
#include <string>

using namespace proxyfmu::server;

namespace
{

void write_data(std::string const& fileName, std::string const& data)
{
    FILE* file = fopen(fileName.c_str(), "wb");
    [[maybe_unused]] size_t bytes_written = fwrite(data.c_str(), sizeof(unsigned char), data.size(), file);
    fclose(file);
}

} // namespace

boot_service_handler::boot_service_handler()
    : rng_(49152, 65535)
{
}

int32_t boot_service_handler::loadFromBinaryData(const std::string& fmuName, const std::string& instanceName, const std::string& data)
{
    auto tmp = std::make_unique<temp_dir>(fmuName);
    std::string fmuPath(tmp->path().string() + "/" + fmuName + ".fmu");

    write_data(fmuPath, data);

    const int port = rng_.next();
    auto t = std::make_unique<std::thread>(&start_process, fmuPath, instanceName, port);
    processes_.emplace_back(std::move(t));

    dirs_.emplace_back(std::move(tmp));

    return port;
}

boot_service_handler::~boot_service_handler()
{
    for (auto& t : processes_) {
        t->join();
    }
}
