
#ifndef PROXY_FMU_FS_PORTABILITY_HPP
#define PROXY_FMU_FS_PORTABILITY_HPP


#if __has_include(<filesystem>)
#    include <filesystem>
namespace proxyfmu
{
namespace filesystem = std::filesystem;
}
#else
#    include <experimental/filesystem>
namespace proxyfmu
{
namespace filesystem = std::experimental::filesystem;
}
#endif

#endif //PROXY_FMU_FS_PORTABILITY_HPP
