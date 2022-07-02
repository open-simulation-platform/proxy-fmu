
link_libraries(proxyfmu-client Catch2::Catch2)

add_executable(test_identity test_identity.cpp)
add_test(NAME test_identity COMMAND test_identity)

add_executable(test_controlled_temp test_controlled_temp.cpp)
add_test(NAME test_controlled_temp COMMAND test_controlled_temp)

add_executable(test_mass-spring-damper test_mass-spring-damper.cpp)
add_test(NAME test_mass-spring-damper COMMAND test_mass-spring-damper)
