
include_directories(${PROXYFMU_SOURCE_DIR}/include)
include_directories(${PROXYFMU_SOURCE_DIR}/src)

###########################################################
#########################FMI############################

add_executable(fmi-test fmi_test.cpp)
target_link_libraries(fmi-test PUBLIC fmilibcpp::fmilibcpp)

###########################################################
#########################PROXY############################

add_executable(proxy-test proxy_test.cpp)
target_link_libraries(proxy-test PUBLIC proxyfmu-client)
