# Find Fmilibcpp
#
# Find the native Fmilibcpp headers and libraries.
#
#   Fmilibcpp_INCLUDE_DIRS   - where to find Fmilibcpp.h
#   Fmilibcpp_LIBRARIES      - List of libraries when using Fmilibcpp.
#   Fmilibcpp_FOUND          - True if Fmilibcpp found.
#
find_path(Fmilibcpp_INCLUDE_DIR NAMES fmilibcpp/fmu.hpp)
mark_as_advanced(Fmilibcpp_INCLUDE_DIR)

find_library(Fmilibcpp_LIBRARY NAMES fmilibcpp)
mark_as_advanced(Fmilibcpp_LIBRARY)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Fmilibcpp
        REQUIRED_VARS Fmilibcpp_LIBRARY Fmilibcpp_INCLUDE_DIR)

if (Fmilibcpp_FOUND)

    set(Fmilibcpp_INCLUDE_DIRS ${Fmilibcpp_INCLUDE_DIR})

    if (NOT Fmilibcpp_LIBRARIES)
        set(Fmilibcpp_LIBRARIES ${Fmilibcpp_LIBRARY})
    endif ()

    if (NOT TARGET fmilibcpp::fmilibcpp)
        add_library(fmilibcpp::fmilibcpp UNKNOWN IMPORTED)
        set_target_properties(fmilibcpp::fmilibcpp PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES ${Fmilibcpp_INCLUDE_DIR})
        set_property(TARGET fmilibcpp::fmilibcpp APPEND PROPERTY
                IMPORTED_LOCATION "${Fmilibcpp_LIBRARY}")
    endif ()

endif ()
