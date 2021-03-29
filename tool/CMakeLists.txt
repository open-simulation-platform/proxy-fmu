
include_directories(${FMU_PROXY_SOURCE_DIR}/include)

add_executable(fmu_proxy fmu_proxy.cpp)
target_link_libraries(fmu_proxy PRIVATE thrift-server Boost::program_options)
target_include_directories(fmu_proxy PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(fmu_proxy PRIVATE )
if (UNIX)
    target_link_libraries(fmu_proxy PRIVATE pthread)
endif()

add_executable(fmu_proxy_starter fmu_proxy_starter.cpp)
target_link_libraries(fmu_proxy_starter PRIVATE Boost::program_options)
target_include_directories(fmu_proxy_starter PRIVATE ${Boost_INCLUDE_DIRS})

install(TARGETS fmu_proxy DESTINATION bin)
install(TARGETS fmu_proxy_starter DESTINATION bin)
