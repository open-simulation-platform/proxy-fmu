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

set_tests_properties(
        test_identity
        test_controlled_temp
        test_mass-spring-damper
        PROPERTIES
            ENVIRONMENT "PATH=$<SHELL_PATH:$<TARGET_FILE_DIR:proxyfmu-client>\;>$ENV{PATH}"
)