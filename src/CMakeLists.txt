set(includeDir ${FMU_PROXY_SOURCE_DIR}/include)
include_directories(${includeDir})

set(generatedSourcesDir "${CMAKE_BINARY_DIR}/generated")
set(thriftSourcesDir "${generatedSourcesDir}/fmuproxy/thrift")

file(MAKE_DIRECTORY "${generatedSourcesDir}")
file(MAKE_DIRECTORY "${thriftSourcesDir}")

if (NOT THRIFT_EXECUTABLE)
  message(FATAL_ERROR "The thrift compiler was not found. Cannot generate Thrift sources.")
endif ()

set(thriftServiceDefinitions
        "${CMAKE_CURRENT_SOURCE_DIR}/fmuproxy/thrift/defs.thrift"
        "${CMAKE_CURRENT_SOURCE_DIR}/fmuproxy/thrift/service.thrift")

# Generate FMU-proxy classes from Thrift service definitions
foreach (def ${thriftServiceDefinitions})
  execute_process(
          COMMAND "${THRIFT_EXECUTABLE}"
          "--gen" "cpp:no_skeleton"
          "-out" "${thriftSourcesDir}"
          "${def}"
  )
endforeach ()

set(thriftGenerated
        "${thriftSourcesDir}/FmuService.h"
        "${thriftSourcesDir}/FmuService.cpp"
        "${thriftSourcesDir}/defs_types.h"
        "${thriftSourcesDir}/defs_types.cpp")

set(THRIFT_COMMON_SRC
        "${thriftSourcesDir}/FmuService.cpp"
        "${thriftSourcesDir}/defs_types.cpp")

set(THRIFT_CLIENT_SRC
        #fmuproxy/thrift/client/thrift_client_helper.hpp
       #fmuproxy/thrift/client/remote_fmu_slave.cpp
        fmuproxy/thrift/client/thrift_client.cpp)

set(THRIFT_CLIENT_HEADERS
       # ${includeDir}/fmuproxy/thrift/client/remote_fmu_slave.hpp
        ${includeDir}/fmuproxy/thrift/client/thrift_client.hpp)

set(THRIFT_SERVER_SRC
        fmuproxy/thrift/server/fmu_service_handler.cpp
        fmuproxy/thrift/server/thrift_fmu_server.cpp)

set(THRIFT_SERVER_HEADERS
        ${includeDir}/fmuproxy/thrift/server/fmu_service_handler.hpp
        ${includeDir}/fmuproxy/thrift/server/thrift_fmu_server.hpp)

set(FMI_SRC
        fmi/fmu.cpp

        fmi/fmi1/fmi1_fmu.cpp
        fmi/fmi1/fmi1_slave.cpp
        fmi/fmi1/fmi1_model_description.cpp

        fmi/fmi2/fmi2_fmu.cpp
        fmi/fmi2/fmi2_slave.cpp
        fmi/fmi2/fmi2_model_description.cpp
        )

add_library(fmi ${FMI_SRC})
target_compile_features(fmi PUBLIC "cxx_std_17")
target_link_libraries(fmi PRIVATE ${FMILibrary_LIBRARIES})
target_include_directories(fmi PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_definitions(fmi PRIVATE "BOOST_ALL_DYN_LINK=1" "BOOST_ALL_NO_LIB=1" "BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE=1")

if (WIN32)
  target_link_libraries(fmi PUBLIC "Bcrypt")
elseif (UNIX)
  target_link_libraries(fmi PRIVATE stdc++fs dl)
endif ()

add_library(thrift-common ${THRIFT_COMMON_SRC})
target_link_libraries(thrift-common PUBLIC thrift::thrift)
target_include_directories(thrift-common PUBLIC ${generatedSourcesDir})

add_library(thrift-client ${THRIFT_CLIENT_SRC} ${THRIFT_CLIENT_HEADERS})
target_link_libraries(thrift-client PUBLIC thrift-common fmi)

add_library(thrift-server ${THRIFT_SERVER_SRC} ${THRIFT_SERVER_HEADERS})
target_link_libraries(thrift-server PUBLIC thrift-common fmi)
if (UNIX)
  target_link_libraries(thrift-server PRIVATE pthread)
endif ()
