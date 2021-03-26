
#ifndef FMU_PROXY_TEMP_DIR_HPP
#define FMU_PROXY_TEMP_DIR_HPP

#include <filesystem>
#include <iostream>
#include <memory>

namespace fmi
{

class temp_dir
{
private:
    const std::filesystem::path path_;

public:
    explicit temp_dir(std::filesystem::path path)
        : path_(std::move(path))
    {
    }

    std::filesystem::path path() const
    {
        return path_;
    }

    ~temp_dir()
    {
        std::cout << "delete temp" << std::endl;
        std::filesystem::remove_all(path_);
    }
};

} // namespace fmi


#endif //FMU_PROXY_TEMP_DIR_HPP
