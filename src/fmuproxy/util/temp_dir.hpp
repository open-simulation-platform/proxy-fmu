
#ifndef FMU_PROXY_TEMP_DIR_HPP
#define FMU_PROXY_TEMP_DIR_HPP

#include <filesystem>
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

namespace fmuproxy::util
{

class temp_dir
{
private:
    const std::filesystem::path path_;

public:
    explicit temp_dir(const std::string& name)
        : path_(std::filesystem::temp_directory_path() /= "proxy_server_" + name + "_" + generate_simple_id(6))
    {
        std::filesystem::create_directories(path_);
    }

    [[nodiscard]] std::filesystem::path path()
    {
        return path_;
    }

    ~temp_dir()
    {
        std::error_code status;
        std::filesystem::remove_all(path_, status);
        if (status) {
            std::cerr << "Failed to remove temp folder '" << path_.string() << "' " << status.message() << std::endl;
        }
    }
};

} // namespace fmi


#endif //FMU_PROXY_TEMP_DIR_HPP
