
include_directories(${PROXYFMU_SOURCE_DIR}/include)

add_executable(proxyfmu proxyfmu.cpp)
target_link_libraries(proxyfmu PRIVATE proxyfmu-lib)
if (UNIX)
  target_link_libraries(proxyfmu PRIVATE pthread)
endif ()


add_executable(proxyfmu_booter proxyfmu_booter.cpp)
target_link_libraries(proxyfmu_booter PRIVATE proxyfmu-booter-lib)
if (UNIX)
  target_link_libraries(proxyfmu_booter PRIVATE pthread)
endif ()

install(
        TARGETS
        "proxyfmu"
        "proxyfmu_booter"
        DESTINATION ${PROXYFMU_INSTALL_DESTINATIONS}
)

if (PROXYFMU_BUILD_TESTS)
  # Copy executable to test directory in order for CTest to function
  add_custom_command(TARGET proxyfmu POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:proxyfmu> ${CMAKE_BINARY_DIR}/tests/$<TARGET_FILE_NAME:proxyfmu>
          )
  add_custom_command(TARGET proxyfmu POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E echo
          Copying $<TARGET_FILE:proxyfmu> into ${CMAKE_BINARY_DIR}/tests/$<TARGET_FILE_NAME:proxyfmu>
          )
endif()
