
#ifndef FMU_PROXY_TEMP_DIR_HPP
#define FMU_PROXY_TEMP_DIR_HPP

#include <filesystem>
#include <iostream>
#include <memory>

#include "simple_id.hpp"

namespace fmi
{

class temp_dir
{
private:
    const std::filesystem::path path_;

public:
    explicit temp_dir(const std::string& name)
        : path_(std::filesystem::temp_directory_path() /= "fmu_proxy_" + name + "_" + generate_simple_id(6))
    {
        std::filesystem::create_directories(path_);
        std::cout << "temp_dir: " << name << std::endl;
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
