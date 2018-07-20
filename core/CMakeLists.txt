
# Collect source files into the "sources" variable and unit test files into the
# "tests" variable
ign_get_libsources_and_unittests(sources tests)

# Create the library target
ign_create_core_library(
  SOURCES ${sources}
  CXX_STANDARD 11)

# No libraries to link

# Build the unit tests
ign_build_tests(
  TYPE UNIT
  SOURCES ${tests})


if(TARGET UNIT_PluginLoader_TEST)
  # Tell the PluginLoader unit test about the location of the library directory
  # so that it can find the library file for testing purposes.
  target_compile_definitions(UNIT_PluginLoader_TEST PRIVATE
    "IGN_COMMON_LIB_PATH=\"$<TARGET_FILE_DIR:${PROJECT_LIBRARY_TARGET_NAME}>\"")
endif()
