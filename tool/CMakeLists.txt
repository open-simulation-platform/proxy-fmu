
include_directories(${PROXYFMU_SOURCE_DIR}/include)

add_executable(proxy_server proxy_fmu.cpp)
target_link_libraries(proxy_server PRIVATE proxy-lib Boost::program_options)
if (UNIX)
  target_link_libraries(proxy_server PRIVATE pthread)
endif ()


add_executable(proxy_server_booter proxy_fmu_booter.cpp)
target_link_libraries(proxy_server_booter PRIVATE proxy-booter-lib Boost::program_options)
if (UNIX)
  target_link_libraries(proxy_server_booter PRIVATE pthread)
endif ()


install(
        TARGETS
        "proxy_server"
        "proxy_server_booter"
        DESTINATION ${PROXYFMU_INSTALL_DESTINATIONS}
)
