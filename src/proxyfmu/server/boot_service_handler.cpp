
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

}



int64_t boot_service_handler::loadFromBinaryData(const std::string& name, const std::string& data)
{
    return 0;
}
