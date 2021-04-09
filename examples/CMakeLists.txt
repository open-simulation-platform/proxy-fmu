
include_directories(${PROXYFMU_SOURCE_DIR}/include)
include_directories(${PROXYFMU_SOURCE_DIR}/src)

###########################################################
#########################FMI############################

add_executable(fmi-test
		fmi/fmi_test.cpp)
target_link_libraries(fmi-test PUBLIC fmi ${FMILibrary_LIBRARIES})

###########################################################
#########################Thrift############################

add_executable(client_test
        thrift/client_test.cpp)
target_link_libraries(client_test PRIVATE ${FMILibrary_LIBRARIES} proxy-client)
