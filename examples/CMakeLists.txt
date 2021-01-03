
include_directories(${FMU_PROXY_SOURCE_DIR}/include)
include_directories(${FMU_PROXY_SOURCE_DIR}/src)

##########################################################
#########################FMI##############################

add_executable(benchmark
        fmi/benchmark.cpp
        example_util.hpp)
target_link_libraries(benchmark PRIVATE fmi4cpp::fmi4cpp)

###########################################################
#########################Thrift############################

add_executable(thrift_client_test
		thrift/thrift_client_test.cpp)
target_link_libraries(thrift_client_test PRIVATE fmi4cpp::fmi4cpp thrift-client )

add_executable(thrift_server_test
		thrift/thrift_server_test.cpp)
target_link_libraries(thrift_server_test PRIVATE fmi4cpp::fmi4cpp thrift-server)

