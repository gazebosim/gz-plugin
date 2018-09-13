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

  foreach(plugin_target
      IGNBadPluginAlign
      IGNBadPluginAPIVersionNew
      IGNBadPluginAPIVersionOld
      IGNBadPluginNoInfo
      IGNBadPluginSize
      IGNDummyPlugins
      IGNFactoryPlugins
      IGNTemplatedPlugins)

    target_compile_definitions(${test} PRIVATE
      "${plugin_target}_LIB=\"$<TARGET_FILE:${plugin_target}>\"")

  endforeach()

endforeach()

foreach(test
    INTEGRATION_EnablePluginFromThis_TEST
    INTEGRATION_factory
    INTEGRATION_plugin
    INTEGRATION_WeakPluginPtr)

  if(TARGET ${test})
    target_link_libraries(${test} ${DL_TARGET})
  endif()

endforeach()
