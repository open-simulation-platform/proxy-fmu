add_compile_definitions(PROXYFMU_DATA_DIR="${CMAKE_SOURCE_DIR}/data")

include_directories(${PROXYFMU_SOURCE_DIR}/include)
include_directories(${PROXYFMU_SOURCE_DIR}/src)

###########################################################
#########################FMI############################

add_executable(fmi-test fmi_test.cpp)
target_link_libraries(fmi-test PRIVATE fmilibwrapper CLI11::CLI11)

###########################################################
#########################PROXY############################

add_executable(proxy-test proxy_test.cpp)
target_link_libraries(proxy-test PRIVATE proxyfmu-client CLI11::CLI11)
