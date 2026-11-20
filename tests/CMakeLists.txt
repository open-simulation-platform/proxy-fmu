include(ExternalProject)

add_compile_definitions(PROXYFMU_DATA_DIR="${CMAKE_SOURCE_DIR}/data")
link_libraries(proxyfmu-client Catch2::Catch2)

# Reference FMUs are built from source, so that we have test FMUs for every
# platform we build on.
set(REFERENCE_FMUS_REPOSITORY
        "https://github.com/open-simulation-platform/Reference-FMUs.git"
        CACHE STRING "Reference-FMUs repository used by tests")
set(REFERENCE_FMUS_VERSION
        "v0.0.42"
        CACHE STRING "Reference-FMUs release tag used by tests")
set(referenceFmusBinaryDir "${CMAKE_CURRENT_BINARY_DIR}/reference-fmus")

set(referenceFmusArgs
        "-DFMI_VERSION=2"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
set(referenceFmusArchitecture "${CMAKE_SYSTEM_PROCESSOR}")
if (APPLE)
    # Reference-FMUs picks the architecture of the host, so tell it which
    # one we build for, e.g. x86_64 on an Apple silicon host.
    if (CMAKE_OSX_ARCHITECTURES)
        set(referenceFmusArchitecture "${CMAKE_OSX_ARCHITECTURES}")
    endif ()
    if (referenceFmusArchitecture MATCHES "^(arm64|aarch64)$")
        list(APPEND referenceFmusArgs
                "-DFMI_ARCHITECTURE=aarch64"
                "-DCMAKE_OSX_ARCHITECTURES=arm64")
    elseif (referenceFmusArchitecture STREQUAL "x86_64")
        list(APPEND referenceFmusArgs
                "-DFMI_ARCHITECTURE=x86_64"
                "-DCMAKE_OSX_ARCHITECTURES=x86_64")
    else ()
        message(FATAL_ERROR "No Reference FMUs for macOS on ${referenceFmusArchitecture}")
    endif ()
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

# Many FMI 2 FMUs keep the binaries for other architectures in an x86
# platform folder, e.g. a universal library in darwin64, and loadFmu() falls
# back to those. To test this, build the Reference FMUs once more with such
# binaries.
set(referenceFmusX86FolderArgs)
if (APPLE AND referenceFmusArchitecture MATCHES "^(arm64|aarch64)$")
    set(referenceFmusX86Folder "darwin64")
    set(referenceFmusX86FolderArgs
            "-DFMI_ARCHITECTURE=x86_64"
            "-DCMAKE_OSX_ARCHITECTURES=arm64|x86_64")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND referenceFmusArchitecture MATCHES "^(arm64|aarch64)$")
    # On Linux, the architecture only selects the folder, so this puts
    # 64-bit ARM code in linux64.
    set(referenceFmusX86Folder "linux64")
    set(referenceFmusX86FolderArgs "-DFMI_ARCHITECTURE=x86_64")
endif ()
if (referenceFmusX86FolderArgs)
    set(referenceFmusX86FolderBinaryDir "${CMAKE_CURRENT_BINARY_DIR}/reference-fmus-${referenceFmusX86Folder}")
    ExternalProject_Get_Property(reference_fmus SOURCE_DIR)
    set(referenceFmusSourceDir "${SOURCE_DIR}")
    ExternalProject_Add(reference_fmus_x86_folder
            DOWNLOAD_COMMAND ""
            SOURCE_DIR "${referenceFmusSourceDir}"
            BINARY_DIR "${referenceFmusX86FolderBinaryDir}"
            LIST_SEPARATOR "|"
            CMAKE_ARGS
                "-DFMI_VERSION=2"
                "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
                ${referenceFmusX86FolderArgs}
            INSTALL_COMMAND ""
            TEST_COMMAND ""
            DEPENDS reference_fmus)
    set(dahlquistX86FolderFmu "${referenceFmusX86FolderBinaryDir}/fmus/Dahlquist.fmu")
    add_dependencies(test_dahlquist reference_fmus_x86_folder)
    target_compile_definitions(test_dahlquist PRIVATE FMU_WITH_NATIVE_CODE_IN_X86_FOLDER="${dahlquistX86FolderFmu}")
endif ()
if (APPLE AND referenceFmusArchitecture MATCHES "^(arm64|aarch64)$")
    # A copy of the universal Dahlquist FMU with only x86_64 code, to check
    # that loadFmu() only falls back to darwin64 binaries with arm64 code.
    set(dahlquistX86Fmu "${referenceFmusX86FolderBinaryDir}/fmus/Dahlquist_x86_64.fmu")
    set(dahlquistX86Dir "${CMAKE_CURRENT_BINARY_DIR}/dahlquist-x86_64")
    set(dahlquistX86Library "${dahlquistX86Dir}/binaries/darwin64/Dahlquist.dylib")
    add_custom_command(
            OUTPUT "${dahlquistX86Fmu}"
            COMMAND "${CMAKE_COMMAND}" -E rm -rf "${dahlquistX86Dir}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${dahlquistX86Dir}"
            COMMAND "${CMAKE_COMMAND}" -E chdir "${dahlquistX86Dir}"
                "${CMAKE_COMMAND}" -E tar xf "${dahlquistX86FolderFmu}"
            COMMAND lipo "${dahlquistX86Library}" -thin x86_64 -output "${dahlquistX86Library}.thin"
            COMMAND "${CMAKE_COMMAND}" -E rename "${dahlquistX86Library}.thin" "${dahlquistX86Library}"
            COMMAND "${CMAKE_COMMAND}" -E chdir "${dahlquistX86Dir}"
                "${CMAKE_COMMAND}" -E tar cf "${dahlquistX86Fmu}" --format=zip modelDescription.xml binaries
            DEPENDS reference_fmus_x86_folder "${dahlquistX86FolderFmu}"
            VERBATIM)
    add_custom_target(dahlquist_x86_64_fmu DEPENDS "${dahlquistX86Fmu}")
    add_dependencies(test_dahlquist dahlquist_x86_64_fmu)
    target_compile_definitions(test_dahlquist PRIVATE FMU_WITHOUT_NATIVE_CODE="${dahlquistX86Fmu}")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND referenceFmusArchitecture MATCHES "^(arm64|aarch64)$")
    # ControlledTemperature.fmu's linux64 library is x86_64 code.
    target_compile_definitions(test_dahlquist PRIVATE
            FMU_WITHOUT_NATIVE_CODE="${CMAKE_SOURCE_DIR}/data/fmus/2.0/20sim/ControlledTemperature.fmu")
endif ()
add_test(NAME test_dahlquist COMMAND test_dahlquist)
set(tests test_dahlquist)

# The following tests use FMUs which only contain binaries for x86 Linux and
# Windows.
if (NOT APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
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

# Run in a directory without the proxyfmu executable, with the
# proxyfmu-client library loaded from a directory that also contains the
# executable: the executable is found next to the library.
set(bundleDir "${CMAKE_CURRENT_BINARY_DIR}/client-bundle")
set(bundleUserDir "${CMAKE_CURRENT_BINARY_DIR}/client-bundle-user")
add_executable(test_executable_next_to_client test_executable_next_to_client.cpp)
add_dependencies(test_executable_next_to_client reference_fmus proxyfmu)
target_compile_definitions(test_executable_next_to_client PRIVATE REFERENCE_FMUS_DIR="${referenceFmusBinaryDir}/fmus")
set_target_properties(test_executable_next_to_client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${bundleUserDir}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${bundleUserDir}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${bundleUserDir}"
        # The library must come from the bundle directory, not the build tree.
        SKIP_BUILD_RPATH ON)
add_custom_command(TARGET test_executable_next_to_client POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${bundleDir}"
        # The client library with the libraries it needs from this project,
        # and the executable.
        COMMAND "${CMAKE_COMMAND}" -E copy
            "$<TARGET_FILE:proxyfmu-client>" "$<TARGET_FILE:fmilibwrapper>" "$<TARGET_FILE:proxyfmu>"
            "${bundleDir}")
add_test(NAME test_executable_next_to_client COMMAND test_executable_next_to_client WORKING_DIRECTORY "${bundleUserDir}")
if (WIN32)
    set(bundlePathVariable "PATH")
elseif (APPLE)
    set(bundlePathVariable "DYLD_LIBRARY_PATH")
else ()
    set(bundlePathVariable "LD_LIBRARY_PATH")
endif ()
set_tests_properties(test_executable_next_to_client PROPERTIES
        TIMEOUT 60
        ENVIRONMENT_MODIFICATION "${bundlePathVariable}=path_list_prepend:${bundleDir}")

set_tests_properties(
        ${tests}
        PROPERTIES
            ENVIRONMENT "PATH=$<SHELL_PATH:$<TARGET_FILE_DIR:proxyfmu-client>\;>$ENV{PATH}"
)
