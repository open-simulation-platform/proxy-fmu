link_directories(${Boost_INCLUDE_DIRS})
link_libraries(proxyfmu-client ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_executable(test_identity test_identity.cpp)
