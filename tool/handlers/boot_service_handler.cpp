
#include "boot_service_handler.hpp"

#include "proxyfmu/process_helper.hpp"

#include <chrono>
#include <cstdio>

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

int32_t boot_service_handler::loadFromBinaryData(const std::string& fmuName, const std::string& instanceName, const std::string& data)
{
    auto tmp = std::make_unique<temp_dir>(fmuName);
    std::string fmuPath(tmp->path().string() + "/" + fmuName + ".fmu");

    write_data(fmuPath, data);

    auto state = std::make_shared<process_state>();
    auto t = std::make_unique<std::thread>(&start_process, fmuPath, instanceName, state);
    processes_.emplace_back(std::move(t));
    dirs_.emplace_back(std::move(tmp));

    std::unique_lock<std::mutex> lck(state->mtx);
    state->cv.wait(lck, [&state] { return state->port != -1; });
    if (state->error) {
        // The client only gets the port, so report the reason here.
        try {
            std::rethrow_exception(state->error);
        } catch (const std::exception& e) {
            std::cerr << "[proxyfmu] Unable to start proxy process: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "[proxyfmu] Unable to start proxy process" << std::endl;
        }
    }

    return state->port;
}

boot_service_handler::~boot_service_handler()
{
    for (auto& t : processes_) {
        if (t->joinable()) t->join();
    }
}
