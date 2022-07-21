
add_executable(test_identity test_identity.cpp)
add_test(NAME test_identity COMMAND test_identity)
target_link_libraries(test_identity PRIVATE proxyfmu-client Catch2::Catch2)

add_executable(test_controlled_temp test_controlled_temp.cpp)
add_test(NAME test_controlled_temp COMMAND test_controlled_temp)
target_link_libraries(test_controlled_temp PRIVATE proxyfmu-client Catch2::Catch2)
