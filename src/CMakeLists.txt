add_subdirectory(thrift)

set(publicHeaderDir ${CMAKE_SOURCE_DIR}/include)
set(generatedSourcesDir "${CMAKE_BINARY_DIR}/generated")

# ==============================================================================
# common headers
# ==============================================================================

set(commonPublicHeaders

        "proxyfmu/fixed_range_random_generator.hpp"
        "proxyfmu/fs_portability.hpp"
        "proxyfmu/lib_info.hpp"
        "proxyfmu/remote_info.hpp"
        "proxyfmu/temp_dir.hpp"

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
target_link_libraries(fmilibwrapper PRIVATE FMILIB::FMILIB)
if (UNIX)
    target_link_libraries(fmilibwrapper INTERFACE stdc++fs PRIVATE dl)
endif ()

# Generate sources from .in files
set(libInfo "${generatedSourcesDir}/proxyfmu/lib_info.cpp")
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/proxyfmu/lib_info.cpp.in"
        "${libInfo}"
        @ONLY
)


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

add_library(proxyfmu-client ${commonPublicHeadersFull} ${clientPublicHeadersFull} ${clientPrivateHeaders} ${clientSources} ${libInfo} "$<TARGET_OBJECTS:proxyfmu-service>")
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
        #PRIVATE # Temporarily disabled due to https://github.com/conan-io/conan/issues/13302
        thrift::thrift
        Boost::filesystem
        )
if (UNIX)
    target_link_libraries(proxyfmu-client PRIVATE pthread)
endif ()

# Make sure executable is up to date
add_dependencies(proxyfmu-client proxyfmu)


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
