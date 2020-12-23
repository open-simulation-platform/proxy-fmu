set(includeDir ${FMU_PROXY_SOURCE_DIR}/include)
include_directories(${includeDir})

set(generatedSourcesDir "${CMAKE_BINARY_DIR}/generated")
set(thriftSourcesDir "${generatedSourcesDir}/fmuproxy/thrift")

file(MAKE_DIRECTORY "${generatedSourcesDir}")
file(MAKE_DIRECTORY "${thriftSourcesDir}")

if (NOT THRIFT_EXECUTABLE)
    message(FATAL_ERROR "The thrift compiler was not found. Cannot generate Thrift sources.")
endif ()
set(thriftServiceDefinition "${CMAKE_CURRENT_SOURCE_DIR}/fmuproxy/thrift/service.thrift")

# Generate FMU-proxy classes from Thrift service definitions
execute_process(
        COMMAND "${THRIFT_EXECUTABLE}"
        "--gen" "cpp:no_skeleton"
        "-out" "${thriftSourcesDir}"
        "${thriftServiceDefinition}"
)

set(thriftGenerated
        "${thriftSourcesDir}/FmuService.h"
        "${thriftSourcesDir}/FmuService.cpp"
        "${thriftSourcesDir}/service_types.h"
        "${thriftSourcesDir}/service_types.cpp"
        )

set(THRIFT_COMMON_SRC
        ${thriftSourcesDir}/FmuService.cpp
        ${thriftSourcesDir}/service_types.cpp)

set(THRIFT_CLIENT_SRC
        fmuproxy/thrift/client/thrift_client_helper.hpp
        fmuproxy/thrift/client/remote_fmu_slave.cpp
        fmuproxy/thrift/client/thrift_client.cpp)

set(THRIFT_CLIENT_HEADERS
        ${includeDir}/fmuproxy/thrift/client/remote_fmu_slave.hpp
        ${includeDir}/fmuproxy/thrift/client/thrift_client.hpp)

set(THRIFT_SERVER_SRC
        fmuproxy/thrift/server/thrift_server_helper.hpp
        fmuproxy/thrift/server/fmu_service_handler.cpp
        fmuproxy/thrift/server/thrift_fmu_server.cpp)

set(THRIFT_SERVER_HEADERS
        ${includeDir}/fmuproxy/thrift/server/fmu_service_handler.hpp
        ${includeDir}/fmuproxy/thrift/server/thrift_fmu_server.hpp)

add_library(thrift-common ${THRIFT_COMMON_SRC})
target_link_libraries(thrift-common PUBLIC thrift::thrift)
target_include_directories(thrift-common PUBLIC ${generatedSourcesDir})

add_library(thrift-client ${THRIFT_CLIENT_SRC} ${THRIFT_CLIENT_HEADERS})
target_link_libraries(thrift-client PUBLIC thrift-common fmi4cpp::fmi4cpp)

add_library(thrift-server ${THRIFT_SERVER_SRC} ${THRIFT_SERVER_HEADERS})
target_link_libraries(thrift-server PUBLIC thrift-common fmi4cpp::fmi4cpp)
if (UNIX)
    target_link_libraries(thrift-server PRIVATE pthread)
endif ()
