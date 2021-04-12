set(publicHeaderDir ${CMAKE_SOURCE_DIR}/include)

#######################COMMON##############################

set(commonPublicHeaders
        "proxyfmu/fs_portability.hpp"
        "proxyfmu/temp_dir.hpp"
        "proxyfmu/fixed_range_random_generator.hpp"
        "proxyfmu/remote_info.hpp"
        )

set(commonPublicHeadersFull)
foreach (header IN LISTS commonPublicHeaders)
  list(APPEND commonPublicHeadersFull "${publicHeaderDir}/${header}")
endforeach ()

#######################FMI##############################

set(fmiPublicHeaders
        "proxyfmu/fmi/fmu.hpp"
        "proxyfmu/fmi/slave.hpp"
        "proxyfmu/fmi/scalar_variable.hpp"
        "proxyfmu/fmi/model_description.hpp"
        )

set(fmiPrivateHeaders
        "proxyfmu/fmi/fmicontext.hpp"

        "proxyfmu/fmi/fmi1/fmi1_fmu.hpp"
        "proxyfmu/fmi/fmi1/fmi1_model_description.hpp"
        "proxyfmu/fmi/fmi1/fmi1_slave.hpp"

        "proxyfmu/fmi/fmi2/fmi2_fmu.hpp"
        "proxyfmu/fmi/fmi2/fmi2_model_description.hpp"
        "proxyfmu/fmi/fmi2/fmi2_slave.hpp"
        )

set(fmiSources
        "proxyfmu/fmi/fmu.cpp"

        "proxyfmu/fmi/fmi1/fmi1_fmu.cpp"
        "proxyfmu/fmi/fmi1/fmi1_slave.cpp"
        "proxyfmu/fmi/fmi1/fmi1_model_description.cpp"

        "proxyfmu/fmi/fmi2/fmi2_fmu.cpp"
        "proxyfmu/fmi/fmi2/fmi2_slave.cpp"
        "proxyfmu/fmi/fmi2/fmi2_model_description.cpp"
        )

set(fmiPublicHeadersFull)
foreach (header IN LISTS fmiPublicHeaders)
  list(APPEND fmiPublicHeadersFull "${publicHeaderDir}/${header}")
endforeach ()

add_library(fmi ${commonPublicHeadersFull} ${fmiPublicHeadersFull} ${fmiPrivateHeaders} ${fmiSources})
add_library(proxyfmu::fmi ALIAS fmi)
target_compile_features(fmi PUBLIC "cxx_std_17")
target_include_directories(fmi
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )
target_link_libraries(fmi PRIVATE ${FMILibrary_LIBRARIES})
if (UNIX)
  target_link_libraries(fmi PRIVATE stdc++fs)
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
        "${CMAKE_CURRENT_SOURCE_DIR}/proxyfmu/thrift/boot.thrift"
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

set(thriftGenerated
        "${thriftSourcesDir}/defs_types.h"
        "${thriftSourcesDir}/defs_types.cpp"
        "${thriftSourcesDir}/FmuService.h"
        "${thriftSourcesDir}/FmuService.cpp"
        "${thriftSourcesDir}/BootService.h"
        "${thriftSourcesDir}/BootService.cpp"
        )

add_library(proxyfmu-service OBJECT ${thriftGenerated})
set_target_properties(proxyfmu-service PROPERTIES POSITION_INDEPENDENT_CODE ON)
list(APPEND generatedFiles "$<TARGET_OBJECTS:proxyfmu-service>")

set(clientPublicHeaders
        "proxyfmu/client/proxy_fmu.hpp")

set(clientPrivateHeaders
        "proxyfmu/client/remote_slave.hpp")

set(clientSources
        "proxyfmu/client/remote_slave.cpp"
        "proxyfmu/client/proxy_fmu.cpp")

set(clientPublicHeadersFull)
foreach (header IN LISTS clientPublicHeaders)
  list(APPEND clientPublicHeadersFull "${publicHeaderDir}/${header}")
endforeach ()

add_library(proxy-client ${commonPublicHeadersFull} ${clientPublicHeadersFull} ${clientPrivateHeaders} ${clientSources} ${generatedFiles})
add_library(proxyfmu::proxy-client ALIAS proxy-client)
target_compile_features(proxy-client PUBLIC "cxx_std_17")
target_include_directories(proxy-client
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${generatedSourcesDir}"
        )
target_link_libraries(proxy-client PUBLIC fmi PRIVATE thrift::thrift)
if (UNIX)
  target_link_libraries(proxy-client PRIVATE pthread)
endif ()

set(serverPublicHeaders
        "proxyfmu/server/fmu_service_handler.hpp")

set(serverSources
        "proxyfmu/server/fmu_service_handler.cpp")

set(serverPublicHeadersFull)
foreach (header IN LISTS serverPublicHeaders)
  list(APPEND serverPublicHeadersFull "${publicHeaderDir}/${header}")
endforeach ()

add_library(proxy-lib ${serverPublicHeadersFull} ${serverSources} ${generatedFiles})
add_library(proxyfmu::proxy-lib ALIAS proxy-lib)
target_compile_features(proxy-lib PUBLIC "cxx_std_17")
target_include_directories(proxy-lib
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
        "${generatedSourcesDir}"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )
target_link_libraries(proxy-lib PUBLIC fmi thrift::thrift)
if (UNIX)
  target_link_libraries(proxy-lib PRIVATE pthread)
endif ()

set(serverBooterPublicHeaders
        "proxyfmu/server/boot_service_handler.hpp")

set(serverBooterSources
        "proxyfmu/server/boot_service_handler.cpp")

set(serverBooterPublicHeadersFull)
foreach (header IN LISTS serverBooterPublicHeaders)
  list(APPEND serverBooterPublicHeadersFull "${publicHeaderDir}/${header}")
endforeach ()

add_library(proxy-booter-lib ${commonPublicHeadersFull} ${serverBooterPublicHeadersFull} ${serverBooterSources} ${generatedFiles})
add_library(proxyfmu::proxy-booter-lib ALIAS proxy-booter-lib)
target_compile_features(proxy-booter-lib PUBLIC "cxx_std_17")
target_include_directories(proxy-booter-lib
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
        "${generatedSourcesDir}"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )
target_link_libraries(proxy-booter-lib PUBLIC thrift::thrift)
if (UNIX)
  target_link_libraries(proxy-booter-lib PRIVATE pthread stdc++fs)
endif ()

# ==============================================================================
# Installation rules
# ==============================================================================

install(
        TARGETS fmi proxy-client
        EXPORT "${PROXYFMU_EXPORT_TARGET}"
        ${PROXYFMU_INSTALL_DESTINATIONS}
)
install(
        DIRECTORY "${publicHeaderDir}/proxyfmu"
        DESTINATION "${PROXYFMU_HEADER_INSTALL_DIR}"
)
