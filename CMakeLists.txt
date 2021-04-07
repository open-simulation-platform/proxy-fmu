
cmake_minimum_required(VERSION 3.11)
file(STRINGS "${CMAKE_SOURCE_DIR}/version.txt" projectVersion)
project("proxy-fmu" VERSION ${projectVersion})
message("Current proxy-fmu version: ${CMAKE_PROJECT_VERSION}\n")

# ==============================================================================
# Build settings
# ==============================================================================

option(BUILD_SHARED_LIBS OFF)
option(PROXY_FMU_BUILD_EXAMPLES "Build examples" OFF)
option(PROXY_FMU_BUILD_TOOL "Build the server executable(s)" ON)

# ==============================================================================
# Global internal configuration
# ==============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    message(FATAL_ERROR "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first!")
endif()

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    add_definitions("-DNOMINMAX" "-D_WIN32_WINNT=0x600" "-D_CRT_SECURE_NO_WARNINGS")
    add_compile_options("/wd4996" "/wd4251" "/wd4244" "/wd4267")
endif()

set(PROXY_FMU_INSTALL_DESTINATIONS
        ARCHIVE DESTINATION "lib"
        LIBRARY DESTINATION "lib"
        RUNTIME DESTINATION "bin"
        INCLUDES DESTINATION include)

# ==============================================================================
# Dependencies
# ==============================================================================

if (PROXY_FMU_BUILD_TOOL)
    list(APPEND BOOST_COMPONENTS program_options)
endif()

find_package(Boost 1.66 COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
find_package(THRIFT REQUIRED)
find_package(FMILibrary REQUIRED)

# ==============================================================================
# Targets
# ==============================================================================

add_subdirectory(src)

if (PROXY_FMU_BUILD_EXAMPLES)
    file(COPY data/fmus DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

    if (PROXY_FMU_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()

if (PROXY_FMU_BUILD_TOOL)
    add_subdirectory(tool)
endif()


