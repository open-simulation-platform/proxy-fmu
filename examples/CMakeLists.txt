
include_directories(${FMU_PROXY_SOURCE_DIR}/include)
include_directories(${FMU_PROXY_SOURCE_DIR}/src)

###########################################################
#########################FMI############################

add_executable(test
				fmi/test.cpp)
target_link_libraries(test PUBLIC fmi ${FMILibrary_LIBRARIES})

###########################################################
#########################Thrift############################

add_executable(thrift_client_test
		thrift/thrift_client_test.cpp)
target_link_libraries(thrift_client_test PRIVATE ${FMILibrary_LIBRARIES} thrift-client)
