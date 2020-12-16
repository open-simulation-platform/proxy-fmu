
INCLUDE_DIRECTORIES(${FMU_PROXY_SOURCE_DIR}/include/)

SET(THRIFT_COMMON_SRC
        fmuproxy/thrift/common/service_types.cpp
        fmuproxy/thrift/common/fmu_service.cpp)

SET(THRIFT_CLIENT_SRC
        fmuproxy/thrift/client/thrift_client_helper.hpp
        fmuproxy/thrift/client/remote_fmu_slave.cpp
        fmuproxy/thrift/client/thrift_client.cpp)

SET(THRIFT_SERVER_SRC
        fmuproxy/thrift/server/fmu_service_handler.cpp
        fmuproxy/thrift/server/thrift_fmu_server.cpp)

add_library(thrift-common ${THRIFT_COMMON_SRC})
target_link_libraries(thrift-common PUBLIC thrift::thrift)

add_library(thrift-client ${THRIFT_CLIENT_SRC})
target_link_libraries(thrift-client PUBLIC thrift-common fmi4cpp::fmi4cpp)

add_library(thrift-server ${THRIFT_SERVER_SRC})
target_include_directories(thrift-server PRIVATE ${JSON_INCLUDE_DIRS})
target_link_libraries(thrift-server PUBLIC thrift-common fmi4cpp::fmi4cpp)
if (UNIX)
    target_link_libraries(thrift-server PRIVATE pthread)
endif()
