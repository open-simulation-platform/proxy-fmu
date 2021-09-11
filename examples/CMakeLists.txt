
include_directories(${PROXYFMU_SOURCE_DIR}/include)
include_directories(${PROXYFMU_SOURCE_DIR}/src)

###########################################################
#########################FMI############################

add_executable(fmi-test fmi/fmi_test.cpp)
target_link_libraries(fmi-test PUBLIC fmilibwrapper)

###########################################################
#########################Thrift############################

add_executable(client_test client/client_test.cpp)
target_link_libraries(client_test PRIVATE proxyfmu-client)

add_executable(client_demo client/demo.cpp)
target_link_libraries(client_demo PRIVATE proxyfmu-client)
