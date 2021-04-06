set(includeDir ${PROXY_FMU_SOURCE_DIR}/include)
include_directories(${includeDir})

#######################FMI##############################

set(FMI_SRC
        proxyfmu/fmi/fmu.cpp

        proxyfmu/fmi/fmi1/fmi1_fmu.cpp
        proxyfmu/fmi/fmi1/fmi1_slave.cpp
        proxyfmu/fmi/fmi1/fmi1_model_description.cpp

        proxyfmu/fmi/fmi2/fmi2_fmu.cpp
        proxyfmu/fmi/fmi2/fmi2_slave.cpp
        proxyfmu/fmi/fmi2/fmi2_model_description.cpp
        )

add_library(fmi ${FMI_SRC})
target_compile_features(fmi PUBLIC "cxx_std_17")
target_link_libraries(fmi PRIVATE ${FMILibrary_LIBRARIES})
if (UNIX)
  target_link_libraries(fmi PRIVATE stdc++fs dl)
endif ()


#######################THRIFT##############################

set(generatedSourcesDir "${CMAKE_BINARY_DIR}/generated")
set(thriftSourcesDir "${generatedSourcesDir}/proxyfmu/thrift")

file(MAKE_DIRECTORY "${generatedSourcesDir}")
file(MAKE_DIRECTORY "${thriftSourcesDir}")

if (NOT THRIFT_EXECUTABLE)
  message(FATAL_ERROR "The thrift compiler was not found. Cannot generate Thrift sources.")
endif ()

set(thriftServiceDefinitions
        "${CMAKE_CURRENT_SOURCE_DIR}/proxyfmu/thrift/defs.thrift"
        "${CMAKE_CURRENT_SOURCE_DIR}/proxyfmu/thrift/service.thrift")

# Generate PROXY-FMU classes from Thrift service definitions
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
        "${thriftSourcesDir}/FmuService.h")

set(generatedSources
        "${thriftSourcesDir}/defs_types.cpp"
        "${thriftSourcesDir}/FmuService.cpp")

set(THRIFT_CLIENT_SRC
        proxyfmu/client/remote_slave.cpp
        proxyfmu/client/proxy_fmu.cpp)

set(THRIFT_SERVER_SRC
        proxyfmu/server/fmu_service_handler.cpp)

add_library(proxy-client ${generatedSources} ${THRIFT_CLIENT_SRC})
target_link_libraries(proxy-client PUBLIC fmi PRIVATE thrift::thrift)
target_include_directories(proxy-client PRIVATE ${generatedSourcesDir})
if (UNIX)
  target_link_libraries(proxy-client PRIVATE pthread)
endif ()

add_library(proxy-lib ${generatedSources} ${THRIFT_SERVER_SRC})
target_link_libraries(proxy-lib PUBLIC fmi thrift::thrift)
target_include_directories(proxy-lib PUBLIC ${generatedSourcesDir})
if (UNIX)
  target_link_libraries(proxy-lib PRIVATE pthread)
endif ()


# ==============================================================================
# Installation rules
# ==============================================================================

install(
        TARGETS fmi proxy-client
        ${PROXY_FMU_INSTALL_DESTINATIONS}
)
install(
        DIRECTORY "${includeDir}/proxyfmu"
        DESTINATION include
)