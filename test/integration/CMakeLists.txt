# This is to make test_config.h visible
include_directories("${CMAKE_BINARY_DIR}")

gz_get_sources(tests)

gz_build_tests(
  TYPE INTEGRATION
  SOURCES ${tests}
  LIB_DEPS
    ${PROJECT_LIBRARY_TARGET_NAME}-loader
  TEST_LIST test_targets)

foreach(test ${test_targets})

  foreach(plugin_target
      GzBadPluginAlign
      GzBadPluginAPIVersionNew
      GzBadPluginAPIVersionOld
      GzBadPluginNoInfo
      GzBadPluginSize
      GzDummyPlugins
      GzFactoryPlugins
      GzTemplatedPlugins)

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

# The linker option -force_load needs to be specified to ensure that the global
# structs specified in the plugin module still get loaded even without any
# explicit reference to the loaded symbols (only the interfaces are
# referenced).
target_link_libraries(INTEGRATION_static_plugins -force_load GzDummyStaticPlugin)
