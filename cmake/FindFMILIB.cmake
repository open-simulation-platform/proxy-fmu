# - CMake script for locating FMI Library
#
# This script is run by the command find_package(FMILibrary). The location of the
# package may be explicitly specified using an environment variable named
# FMILibrary_DIR or a CMake variable of the same name.  If both are specified, the
# latter takes precedence.  The variable should point to the package
# installation prefix, i.e. the directory that contains the "bin", "lib" and
# "include" subdirectories.
#
# The script searches for both static and shared/dynamic libraries, and creates
# IMPORTED targets named "fmilib::static" and "fmilib::shared", respectively.  These
# targets will have several of their IMPORTED_* and INTERFACE_* properties set,
# making explicit use of commands like include_directories() superfluous in most
# cases.
#
# If the variable FMILibrary_USE_SHARED_LIB is set to TRUE, FMILibrary_LIBRARIES will
# contain the name of the IMPORTED target "fmilib::shared". Otherwise, it will
# contain "fmilib::static".
#
# After the script has completed, the variable FMILibrary_FOUND will contain whether
# the package was found or not.  If it was, then the following variables will
# also be set:
#
#    FMILibrary_INCLUDE_DIRS    - The directory that contains the header files.
#    FMILibrary_LIBRARIES       - The name of an IMPORTED target.
#
#    FMILibrary_DLL             - Path to dynamic library (Windows only).
#    FMILibrary_LIBRARY         - Path to static library.
#    FMILibrary_SHARED_LIBRARY  - Path to shared/import library.
#

# Find static library, and use its path prefix to provide a HINTS option to the
# other find_*() commands.
find_path(FMILIB_INCLUDE_DIR NAMES fmilib.h)
mark_as_advanced(FMILIB_INCLUDE_DIR)

find_library(FMILIB_LIBRARY NAMES fmilib fmilib_shared)
mark_as_advanced(FMILIB_LIBRARY)

find_program(FMILIB_EXECUTABLE NAMES FMILIB)
mark_as_advanced(FMILIB_EXECUTABLE)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FMILIB
        REQUIRED_VARS FMILIB_LIBRARY FMILIB_INCLUDE_DIR)

if (FMILIB_FOUND)

  set(FMILIB_INCLUDE_DIRS ${FMILIB_INCLUDE_DIR})

  if (NOT FMILIB_LIBRARIES)
    set(FMILIB_LIBRARIES ${FMILIB_LIBRARY})
  endif ()

  if (NOT TARGET FMILIB::FMILIB)
    add_library(FMILIB::FMILIB UNKNOWN IMPORTED)
    set_target_properties(FMILIB::FMILIB PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES ${FMILIB_INCLUDE_DIR})
    set_property(TARGET FMILIB::FMILIB APPEND PROPERTY
            IMPORTED_LOCATION "${FMILIB_LIBRARY}")
  endif ()

endif ()