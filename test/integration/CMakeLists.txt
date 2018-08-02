# This is to make test_config.h visible
include_directories("${CMAKE_BINARY_DIR}")

ign_get_sources(tests)

ign_build_tests(
  TYPE INTEGRATION
  SOURCES ${tests}
  LIB_DEPS
    ${PROJECT_LIBRARY_TARGET_NAME}-loader
  TEST_LIST test_targets)

foreach(test ${test_targets})

  target_compile_definitions(${test} PRIVATE
    "IGNDummyPlugin_LIB=\"$<TARGET_FILE:IGNDummyPlugins>\"")
  target_compile_definitions(${test} PRIVATE
    "IGNBadPluginAlign_LIB=\"$<TARGET_FILE:IGNBadPluginAlign>\"")
  target_compile_definitions(${test} PRIVATE
    "IGNBadPluginAPIVersionNew_LIB=\"$<TARGET_FILE:IGNBadPluginAPIVersionNew>\"")
  target_compile_definitions(${test} PRIVATE
    "IGNBadPluginAPIVersionOld_LIB=\"$<TARGET_FILE:IGNBadPluginAPIVersionOld>\"")
  target_compile_definitions(${test} PRIVATE
    "IGNBadPluginSize_LIB=\"$<TARGET_FILE:IGNBadPluginSize>\"")

endforeach()
