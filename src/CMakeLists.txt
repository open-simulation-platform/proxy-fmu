set(publicHeaderDir ${CMAKE_SOURCE_DIR}/include)


# ==============================================================================
# common headers
# ==============================================================================

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


# ==============================================================================
# fmilib wrapper
# ==============================================================================

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

add_library(fmilibwrapper ${commonPublicHeadersFull} ${fmiPublicHeadersFull} ${fmiPrivateHeaders} ${fmiSources})
add_library(proxyfmu::fmilibwrapper ALIAS fmilibwrapper)
target_compile_features(fmilibwrapper PUBLIC "cxx_std_17")
set_target_properties(fmilibwrapper PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(fmilibwrapper
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )
target_link_libraries(fmilibwrapper PRIVATE CONAN_PKG::fmilibrary)
if (UNIX)
  target_link_libraries(fmilibwrapper INTERFACE stdc++fs PRIVATE dl)
endif ()


# ==============================================================================
# thrift
# ==============================================================================

set(generatedSourcesDir "${CMAKE_BINARY_DIR}/generated")
set(thriftCppSourcesDir "${generatedSourcesDir}/proxyfmu/thrift")
set(thriftJavaSourcesDir "${generatedSourcesDir}/java")

file(MAKE_DIRECTORY "${generatedSourcesDir}")
file(MAKE_DIRECTORY "${thriftCppSourcesDir}")
file(MAKE_DIRECTORY "${thriftJavaSourcesDir}")

find_program(THRIFT_EXECUTABLE NAMES thrift)
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
          "-out" "${thriftCppSourcesDir}"
          "${def}"
  )
  execute_process(
          COMMAND "${THRIFT_EXECUTABLE}"
          "--gen" "java:private-members,fullcamel"
          "-out" "${thriftJavaSourcesDir}"
          "${def}"
  )
endforeach ()

set(thriftGenerated
        "${thriftCppSourcesDir}/defs_types.h"
        "${thriftCppSourcesDir}/defs_types.cpp"
        "${thriftCppSourcesDir}/FmuService.h"
        "${thriftCppSourcesDir}/FmuService.cpp"
        "${thriftCppSourcesDir}/BootService.h"
        "${thriftCppSourcesDir}/BootService.cpp"
        )

add_library(proxyfmu-service OBJECT ${thriftGenerated})
target_link_libraries(proxyfmu-service PRIVATE CONAN_PKG::thrift)
set_target_properties(proxyfmu-service PROPERTIES POSITION_INDEPENDENT_CODE ON)
list(APPEND generatedFiles "$<TARGET_OBJECTS:proxyfmu-service>")


# ==============================================================================
# proxyfmu client lib
# ==============================================================================

set(clientPublicHeaders
        "proxyfmu/client/proxy_fmu.hpp")

set(clientPrivateHeaders
        "proxyfmu/process_helper.hpp"
        "proxyfmu/client/proxy_slave.hpp")

set(clientSources
        "proxyfmu/client/proxy_slave.cpp"
        "proxyfmu/client/proxy_fmu.cpp")

set(clientPublicHeadersFull)
foreach (header IN LISTS clientPublicHeaders)
  list(APPEND clientPublicHeadersFull "${publicHeaderDir}/${header}")
endforeach ()

add_library(proxyfmu-client ${commonPublicHeadersFull} ${clientPublicHeadersFull} ${clientPrivateHeaders} ${clientSources} ${generatedFiles})
add_library(proxyfmu::proxy-client ALIAS proxyfmu-client)
target_compile_features(proxyfmu-client PUBLIC "cxx_std_17")
set_target_properties(proxyfmu-client PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(proxyfmu-client
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${generatedSourcesDir}"
        )
target_link_libraries(proxyfmu-client
        PUBLIC
        fmilibwrapper
        PRIVATE
        CONAN_PKG::thrift
        Boost::filesystem
        )
if (UNIX)
  target_link_libraries(proxyfmu-client PRIVATE pthread)
endif ()

# Make sure executable is up to date
add_dependencies(proxyfmu-client proxyfmu)


# ==============================================================================
# proxyfmu server lib
# ==============================================================================

set(serverPublicHeaders
        "proxyfmu/server/fmu_service_handler.hpp")

set(serverSources
        "proxyfmu/server/fmu_service_handler.cpp")

set(serverPublicHeadersFull)
foreach (header IN LISTS serverPublicHeaders)
  list(APPEND serverPublicHeadersFull "${publicHeaderDir}/${header}")
endforeach ()

add_library(proxyfmu-lib ${serverPublicHeadersFull} ${serverSources} ${generatedFiles})
add_library(proxyfmu::proxy-lib ALIAS proxyfmu-lib)
target_compile_features(proxyfmu-lib PUBLIC "cxx_std_17")
target_include_directories(proxyfmu-lib
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
        "${generatedSourcesDir}"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )
target_link_libraries(proxyfmu-lib PUBLIC fmilibwrapper CONAN_PKG::thrift)
if (UNIX)
  target_link_libraries(proxyfmu-lib PRIVATE pthread)
endif ()


# ==============================================================================
# proxyfmu booter lib
# ==============================================================================

set(serverBooterPublicHeaders
        "proxyfmu/server/boot_service_handler.hpp")

set(serverBooterPrivateHeaders
        "proxyfmu/process_helper.hpp")

set(serverBooterSources
        "proxyfmu/server/boot_service_handler.cpp")

set(serverBooterPublicHeadersFull)
foreach (header IN LISTS serverBooterPublicHeaders)
  list(APPEND serverBooterPublicHeadersFull "${publicHeaderDir}/${header}")
endforeach ()

add_library(proxyfmu-booter-lib ${commonPublicHeadersFull} ${serverBooterPublicHeadersFull} ${serverBooterPrivateHeaders} ${serverBooterSources} ${generatedFiles})
add_library(proxyfmu::proxyfmu-booter-lib ALIAS proxyfmu-booter-lib)
target_compile_features(proxyfmu-booter-lib PUBLIC "cxx_std_17")
target_include_directories(proxyfmu-booter-lib
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
        "${generatedSourcesDir}"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )
target_link_libraries(proxyfmu-booter-lib
        PUBLIC
        CONAN_PKG::thrift
        PRIVATE
        Boost::filesystem
        )
if (UNIX)
  target_link_libraries(proxyfmu-booter-lib PRIVATE pthread stdc++fs)
endif ()


# ==============================================================================
# Installation rules
# ==============================================================================

install(
        TARGETS fmilibwrapper proxyfmu-client
        EXPORT "${PROXYFMU_EXPORT_TARGET}"
        ${PROXYFMU_INSTALL_DESTINATIONS}
)
install(
        DIRECTORY "${publicHeaderDir}/proxyfmu"
        DESTINATION "${PROXYFMU_HEADER_INSTALL_DIR}"
)

install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/proxyfmu/thrift"
        DESTINATION "."
)

install(
        DIRECTORY "${CMAKE_BINARY_DIR}/generated/java"
        DESTINATION "thrift"
)
