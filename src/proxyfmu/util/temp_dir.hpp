
#ifndef PROXY_FMU_TEMP_DIR_HPP
#define PROXY_FMU_TEMP_DIR_HPP

#include <proxyfmu/fs_portability.hpp>

#include <iostream>
#include <memory>
#include <random>

namespace
{

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> dist(0, 9);

std::string generate_simple_id(const int len)
{
    std::string id;
    for (auto i = 0; i < len; i++) {
        id += std::to_string(dist(mt));
    }
    return id;
}

} // namespace

namespace proxyfmu::util
{

class temp_dir
{
private:
    const filesystem::path path_;

public:
    explicit temp_dir(const std::string& name)
        : path_(filesystem::temp_directory_path() /= "proxy_fmu_" + name + "_" + generate_simple_id(6))
    {
        filesystem::create_directories(path_);
    }

    [[nodiscard]] filesystem::path path()
    {
        return path_;
    }

    ~temp_dir()
    {
        std::error_code status;
        filesystem::remove_all(path_, status);
        if (status) {
            std::cerr << "Failed to remove temp folder '" << path_.string() << "' " << status.message() << std::endl;
        }
    }
};

} // namespace proxyfmu::util


#endif //PROXY_FMU_TEMP_DIR_HPP
