add_compile_definitions(PROXYFMU_DATA_DIR="${CMAKE_SOURCE_DIR}/data")
link_libraries(proxyfmu-client Catch2::Catch2)

add_executable(test_identity test_identity.cpp)
add_test(NAME test_identity COMMAND test_identity)
target_link_libraries(test_identity PRIVATE proxyfmu-client)

add_executable(test_controlled_temp test_controlled_temp.cpp)
add_test(NAME test_controlled_temp COMMAND test_controlled_temp)
target_link_libraries(test_controlled_temp PRIVATE proxyfmu-client)

add_executable(test_mass-spring-damper test_mass-spring-damper.cpp)
add_test(NAME test_mass-spring-damper COMMAND test_mass-spring-damper)

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

set_tests_properties(
        test_identity
        test_controlled_temp
        test_mass-spring-damper
        test_load_failure
        PROPERTIES
            ENVIRONMENT "PATH=$<SHELL_PATH:$<TARGET_FILE_DIR:proxyfmu-client>\;>$ENV{PATH}"
)