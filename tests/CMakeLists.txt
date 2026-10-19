include(ExternalProject)

add_compile_definitions(PROXYFMU_DATA_DIR="${CMAKE_SOURCE_DIR}/data")
link_libraries(proxyfmu-client Catch2::Catch2)

# Reference FMUs are built from source, so that we have test FMUs for every
# platform we build on.
set(REFERENCE_FMUS_REPOSITORY
        "https://github.com/open-simulation-platform/Reference-FMUs.git"
        CACHE STRING "Reference-FMUs repository used by tests")
set(REFERENCE_FMUS_VERSION
        "v0.0.41"
        CACHE STRING "Reference-FMUs release tag used by tests")
set(referenceFmusBinaryDir "${CMAKE_CURRENT_BINARY_DIR}/reference-fmus")

set(referenceFmusArgs
        "-DFMI_VERSION=2"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
if (APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
    # Reference-FMUs v0.0.41 refuses to build FMI 2 FMUs for Apple silicon.
    # As a stopgap, build them for x86_64 ("darwin64") but as universal
    # binaries. They then load on Apple silicon through the fallback from
    # "aarch64-darwin" to "darwin64" in loadFmu(), which this also tests.
    list(APPEND referenceFmusArgs
            "-DFMI_ARCHITECTURE=x86_64"
            "-DCMAKE_C_FLAGS=-arch arm64")
endif ()

ExternalProject_Add(reference_fmus
        GIT_REPOSITORY "${REFERENCE_FMUS_REPOSITORY}"
        GIT_TAG "${REFERENCE_FMUS_VERSION}"
        GIT_SHALLOW ON
        BINARY_DIR "${referenceFmusBinaryDir}"
        CMAKE_ARGS ${referenceFmusArgs}
        INSTALL_COMMAND ""
        TEST_COMMAND "")

add_executable(test_dahlquist test_dahlquist.cpp)
add_dependencies(test_dahlquist reference_fmus)
target_compile_definitions(test_dahlquist PRIVATE REFERENCE_FMUS_DIR="${referenceFmusBinaryDir}/fmus")
add_test(NAME test_dahlquist COMMAND test_dahlquist)
set(tests test_dahlquist)

# The following tests use FMUs which only contain binaries for Linux and
# Windows.
if (NOT APPLE)
    add_executable(test_identity test_identity.cpp)
    add_test(NAME test_identity COMMAND test_identity)
    target_link_libraries(test_identity PRIVATE proxyfmu-client)

    add_executable(test_controlled_temp test_controlled_temp.cpp)
    add_test(NAME test_controlled_temp COMMAND test_controlled_temp)
    target_link_libraries(test_controlled_temp PRIVATE proxyfmu-client)

    add_executable(test_mass-spring-damper test_mass-spring-damper.cpp)
    add_test(NAME test_mass-spring-damper COMMAND test_mass-spring-damper)

    list(APPEND tests
            test_identity
            test_controlled_temp
            test_mass-spring-damper)
endif ()

# A copy of ControlledTemperature.fmu without its binaries
set(noBinariesFmu "${CMAKE_CURRENT_BINARY_DIR}/NoBinaries.fmu")
set(noBinariesDir "${CMAKE_CURRENT_BINARY_DIR}/no-binaries")
set(noBinariesSource "${CMAKE_SOURCE_DIR}/data/fmus/2.0/20sim/ControlledTemperature.fmu")
add_custom_command(
        OUTPUT "${noBinariesFmu}"
        COMMAND "${CMAKE_COMMAND}" -E rm -rf "${noBinariesDir}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${noBinariesDir}"
        COMMAND "${CMAKE_COMMAND}" -E chdir "${noBinariesDir}"
            "${CMAKE_COMMAND}" -E tar xf "${noBinariesSource}" modelDescription.xml
        COMMAND "${CMAKE_COMMAND}" -E chdir "${noBinariesDir}"
            "${CMAKE_COMMAND}" -E tar cf "${noBinariesFmu}" --format=zip modelDescription.xml
        DEPENDS "${noBinariesSource}"
        VERBATIM)
add_custom_target(no_binaries_fmu DEPENDS "${noBinariesFmu}")

add_executable(test_load_failure test_load_failure.cpp)
add_dependencies(test_load_failure no_binaries_fmu)
target_compile_definitions(test_load_failure PRIVATE NO_BINARIES_FMU="${noBinariesFmu}")
add_test(NAME test_load_failure COMMAND test_load_failure)
set_tests_properties(test_load_failure PROPERTIES TIMEOUT 60)

# Built and run in a directory without the proxyfmu executable
set(noExecutableDir "${CMAKE_CURRENT_BINARY_DIR}/no-proxyfmu")
add_executable(test_missing_executable test_missing_executable.cpp)
set_target_properties(test_missing_executable PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${noExecutableDir}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${noExecutableDir}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${noExecutableDir}")
add_test(NAME test_missing_executable COMMAND test_missing_executable WORKING_DIRECTORY "${noExecutableDir}")
set_tests_properties(test_missing_executable PROPERTIES TIMEOUT 60)

list(APPEND tests
        test_load_failure
        test_missing_executable)

set_tests_properties(
        ${tests}
        PROPERTIES
            ENVIRONMENT "PATH=$<SHELL_PATH:$<TARGET_FILE_DIR:proxyfmu-client>\;>$ENV{PATH}"
)
