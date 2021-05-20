
cmake_minimum_required(VERSION 3.9)
file(STRINGS "${CMAKE_SOURCE_DIR}/version.txt" projectVersion)
project("proxyfmu" VERSION ${projectVersion})
message("Current ${PROJECT_NAME} version: ${CMAKE_PROJECT_VERSION}\n")

# ==============================================================================
# Build settings
# ==============================================================================

set(BUILD_SHARED_LIBS OFF)
option(PROXYFMU_BUILD_TESTS "Build tests" OFF)
option(PROXYFMU_BUILD_EXAMPLES "Build examples" OFF)
option(PROXYFMU_BUILD_TOOL "Build the server executable(s)" ON)

# ==============================================================================
# Global internal configuration
# ==============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(Boost_USE_STATIC_LIBS ON)

if (MSVC)
  add_definitions("-D_WIN32_WINNT=0x600" "-D_CRT_SECURE_NO_WARNINGS")
  add_compile_options("/wd4996" "/wd4251" "/wd4244" "/wd4267")
endif ()

# Prepare for export and installation.
set(PROXYFMU_HEADER_INSTALL_DIR "include")
if (WIN32)
  set(PROXYFMU_CMAKE_INSTALL_DIR "cmake")
else ()
  set(PROXYFMU_CMAKE_INSTALL_DIR "share/${PROJECT_NAME}/cmake")
endif ()

set(PROXYFMU_INSTALL_DESTINATIONS
        ARCHIVE DESTINATION "lib"
        LIBRARY DESTINATION "lib"
        RUNTIME DESTINATION "bin"
        INCLUDES DESTINATION "${PROXYFMU_HEADER_INSTALL_DIR}")
set(PROXYFMU_EXPORT_TARGET "${PROJECT_NAME}-targets")


# ==============================================================================
# Dependencies
# ==============================================================================

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
else ()
  message(FATAL_ERROR "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first!")
endif ()

set(BOOST_COMPONENTS system filesystem program_options)
if (PROXYFMU_BUILD_TESTS)
  list(APPEND BOOST_COMPONENTS unit_test_framework)
endif ()

# ==============================================================================
# Targets
# ==============================================================================

add_subdirectory(src)
add_subdirectory(tool)

if (PROXYFMU_BUILD_EXAMPLES OR PROXYFMU_BUILD_TESTS)
  file(COPY data/fmus DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

  if (PROXYFMU_BUILD_EXAMPLES)
    add_subdirectory(examples)
  endif ()

  if (PROXYFMU_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
  endif ()

endif ()


# ==============================================================================
# Exports and remaining installation
# ==============================================================================

install(
        EXPORT "${PROXYFMU_EXPORT_TARGET}"
        DESTINATION "${PROXYFMU_CMAKE_INSTALL_DIR}"
        NAMESPACE "${PROJECT_NAME}::"
)

include(CMakePackageConfigHelpers)

# Generate and install package-config file.
set(configFile "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-config.cmake")
set(targetsFile "${PROXYFMU_CMAKE_INSTALL_DIR}/${PROXYFMU_EXPORT_TARGET}.cmake")
configure_package_config_file(
        "${CMAKE_SOURCE_DIR}/cmake/project-config.cmake.in"
        "${configFile}"
        INSTALL_DESTINATION "${PROXYFMU_CMAKE_INSTALL_DIR}"
        PATH_VARS targetsFile
)
install(FILES "${configFile}" DESTINATION "${PROXYFMU_CMAKE_INSTALL_DIR}")

# Generate and install package-version file
set(versionFile "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake")
write_basic_package_version_file(
        "${versionFile}"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY "SameMajorVersion")
install(FILES "${versionFile}" DESTINATION "${PROXYFMU_CMAKE_INSTALL_DIR}")
