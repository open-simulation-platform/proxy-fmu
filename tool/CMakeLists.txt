
include_directories(${PROXY_SERVER_SOURCE_DIR}/include)

add_executable(proxy_server fmu_proxy.cpp)
target_link_libraries(proxy_server PRIVATE proxy-lib Boost::program_options)
target_include_directories(proxy_server PRIVATE ${Boost_INCLUDE_DIRS})
if (UNIX)
    target_link_libraries(proxy_server PRIVATE pthread)
endif()

install(TARGETS proxy_server DESTINATION bin)
