
include_directories(${PROXYFMU_SOURCE_DIR}/include)

add_executable(proxyfmu proxyfmu.cpp)
target_link_libraries(proxyfmu PRIVATE proxy-lib Boost::program_options)
if (UNIX)
  target_link_libraries(proxyfmu PRIVATE pthread)
endif ()


add_executable(proxyfmu_booter proxyfmu_booter.cpp)
target_link_libraries(proxyfmu_booter PRIVATE proxy-booter-lib Boost::program_options)
if (UNIX)
  target_link_libraries(proxyfmu_booter PRIVATE pthread)
endif ()


install(
        TARGETS
        "proxyfmu"
        "proxyfmu_booter"
        DESTINATION ${PROXYFMU_INSTALL_DESTINATIONS}
)
