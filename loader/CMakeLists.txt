
# Collect source files into the "sources" variable and unit test files into the
# "tests" variable
ign_get_libsources_and_unittests(sources tests)

# Create the library target
ign_add_component(loader
  SOURCES ${sources}
  GET_TARGET_NAME loader)

target_link_libraries(${loader}
  PUBLIC ignition-cmake${IGN_CMAKE_VER}::utilities
  PRIVATE ${DL_TARGET})

ign_build_tests(
  TYPE UNIT
  SOURCES ${tests}
  LIB_DEPS ${loader}
  TEST_LIST test_targets)

foreach(test ${test_targets})

  target_compile_definitions(${test} PRIVATE
    "IGN_PLUGIN_LIB=\"$<TARGET_FILE:${PROJECT_LIBRARY_TARGET_NAME}>\"")

  target_compile_definitions(${test} PRIVATE
    "IGN_PLUGIN_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\"")

  target_compile_definitions(${test} PRIVATE
    "IGNDummyPlugins_LIB=\"$<TARGET_FILE:IGNDummyPlugins>\"")

endforeach()

install(
  DIRECTORY include/
  DESTINATION ${IGN_INCLUDE_INSTALL_DIR_FULL})
