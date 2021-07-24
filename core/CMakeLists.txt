
# Collect source files into the "sources" variable and unit test files into the
# "tests" variable
ign_get_libsources_and_unittests(sources tests)

# Create the library target
ign_create_core_library(
  SOURCES ${sources}
  CXX_STANDARD 17)

target_link_libraries(${PROJECT_LIBRARY_TARGET_NAME}
  PUBLIC ignition-utils${IGN_UTILS_VER}::ignition-utils${IGN_UTILS_VER})

# Build the unit tests
ign_build_tests(
  TYPE UNIT
  SOURCES ${tests})


add_subdirectory(include/ignition/plugin)
