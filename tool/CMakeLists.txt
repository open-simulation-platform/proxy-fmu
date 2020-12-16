
include_directories(${FMU_PROXY_SOURCE_DIR}/include)

add_executable(fmu_proxy fmu_proxy.cpp)
target_link_libraries(fmu_proxy PRIVATE fmi4cpp::fmi4cpp Boost::program_options)

target_include_directories(fmu_proxy PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(fmu_proxy PRIVATE thrift-server)

if (UNIX)
    target_link_libraries(fmu_proxy PRIVATE pthread)
endif()

install(TARGETS fmu_proxy DESTINATION bin)
