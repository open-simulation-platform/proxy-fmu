# Finds the CLI11 library.
#
# If found, the following is defined:
#   - `CLI11_FOUND` and `CLI11_INCLUDE_DIRS` variables.
#   - `CLI11::CLI11` target

find_path(CLI11_INCLUDE_DIR "CLI/CLI.hpp")
if(CLI11_INCLUDE_DIR)
    if (NOT TARGET CLI11::CLI11)
        set(CLI11_INCLUDE_DIRS "${CLI11_INCLUDE_DIR}")
        add_library(CLI11::CLI11 INTERFACE IMPORTED)
        set_target_properties(CLI11::CLI11 PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${CLI11_INCLUDE_DIR}"
                )
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("CLI11" DEFAULT_MSG "CLI11_INCLUDE_DIRS")
