set(includeDir ${FMU_PROXY_SOURCE_DIR}/include)
include_directories(${includeDir})

#######################FMI##############################

set(FMI_SRC
        fmuproxy/fmi/fmu.cpp

        fmuproxy/fmi/fmi1/fmi1_fmu.cpp
        fmuproxy/fmi/fmi1/fmi1_slave.cpp
        fmuproxy/fmi/fmi1/fmi1_model_description.cpp

        fmuproxy/fmi/fmi2/fmi2_fmu.cpp
        fmuproxy/fmi/fmi2/fmi2_slave.cpp
        fmuproxy/fmi/fmi2/fmi2_model_description.cpp
        )

add_library(fmi ${FMI_SRC})
target_compile_features(fmi PUBLIC "cxx_std_17")
target_link_libraries(fmi PRIVATE ${FMILibrary_LIBRARIES})

#######################THRIFT##############################

set(generatedSourcesDir "${CMAKE_BINARY_DIR}/generated")
set(thriftSourcesDir "${generatedSourcesDir}/fmuproxy/thrift")

file(MAKE_DIRECTORY "${generatedSourcesDir}")
file(MAKE_DIRECTORY "${thriftSourcesDir}")

if (NOT THRIFT_EXECUTABLE)
  message(FATAL_ERROR "The thrift compiler was not found. Cannot generate Thrift sources.")
endif ()

set(thriftServiceDefinitions
        "${CMAKE_CURRENT_SOURCE_DIR}/fmuproxy/thrift/defs.thrift"
        "${CMAKE_CURRENT_SOURCE_DIR}/fmuproxy/thrift/service.thrift"
        "${CMAKE_CURRENT_SOURCE_DIR}/fmuproxy/thrift/service_starter.thrift")

# Generate FMU-proxy classes from Thrift service definitions
foreach (def ${thriftServiceDefinitions})
  execute_process(
          COMMAND "${THRIFT_EXECUTABLE}"
          "--gen" "cpp:no_skeleton"
          "-out" "${thriftSourcesDir}"
          "${def}"
  )
endforeach ()

set(generatedHeaders
        "${thriftSourcesDir}/defs_types.h"
        "${thriftSourcesDir}/FmuService.h"
        "${thriftSourcesDir}/FmuServiceStarter.h")

set(generatedSources
        "${thriftSourcesDir}/defs_types.cpp"
        "${thriftSourcesDir}/FmuService.cpp"
        "${thriftSourcesDir}/FmuServiceStarter.cpp")

set(THRIFT_CLIENT_SRC
        fmuproxy/thrift/client/remote_slave.cpp
        fmuproxy/thrift/client/thrift_client.cpp)

set(THRIFT_SERVER_SRC
        fmuproxy/thrift/server/fmu_service_handler.cpp
        fmuproxy/thrift/server/thrift_fmu_server.cpp)

if (UNIX)
  target_link_libraries(fmi PRIVATE stdc++fs dl)
endif ()


add_library(thrift-client ${generatedSources} ${THRIFT_CLIENT_SRC})
target_link_libraries(thrift-client PUBLIC fmi PRIVATE thrift::thrift)
target_include_directories(thrift-client PRIVATE ${generatedSourcesDir})

add_library(thrift-server ${generatedSources} ${THRIFT_SERVER_SRC})
target_link_libraries(thrift-server PUBLIC fmi thrift::thrift)
target_include_directories(thrift-server PUBLIC ${generatedSourcesDir})
if (UNIX)
  target_link_libraries(thrift-server PRIVATE pthread)
endif ()
