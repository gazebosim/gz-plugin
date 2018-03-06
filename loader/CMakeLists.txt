
# Collect source files into the "sources" variable and unit test files into the
# "tests" variable
ign_get_libsources_and_unittests(sources tests)

message(STATUS "loader sources:${sources}")

# Create the library target
ign_add_component(loader
  SOURCES ${sources}
  CXX_STANDARD 11
  GET_TARGET_NAME loader)

target_link_libraries(${loader} PRIVATE ${DL_TARGET})

# Build the unit tests
ign_build_tests(
  TYPE UNIT
  SOURCES ${tests}
  LIB_DEPS ${loader}
  TEST_LIST test_targets)

foreach(test ${test_targets})

  target_compile_definitions(${test}
    PRIVATE "PROJECT_LIBRARY_DIR=\"$<TARGET_FILE_DIR:${loader}>\"")

endforeach()
