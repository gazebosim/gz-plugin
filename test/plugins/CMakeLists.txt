add_library(IGNBadPluginAlign         SHARED BadPluginAlign.cc)
add_library(IGNBadPluginAPIVersionNew SHARED BadPluginAPIVersionNew.cc)
add_library(IGNBadPluginAPIVersionOld SHARED BadPluginAPIVersionOld.cc)
add_library(IGNBadPluginNoInfo        SHARED BadPluginNoInfo.cc)
add_library(IGNBadPluginSize          SHARED BadPluginSize.cc)
add_library(IGNFactoryPlugins         SHARED FactoryPlugins.cc)
add_library(IGNTemplatedPlugins       SHARED TemplatedPlugins.cc)

add_library(IGNDummyPlugins SHARED
  DummyPlugins.cc
  DummyPluginsOtherTranslationUnit.cc)

add_library(IGNLoadsAnotherPlugin SHARED
  LoadsAnotherPlugin.cc)

target_link_libraries(IGNLoadsAnotherPlugin PRIVATE
  ${PROJECT_LIBRARY_TARGET_NAME}-loader)
target_compile_definitions(IGNLoadsAnotherPlugin PRIVATE
  "IGNDummyPlugins_LIB=\"$<TARGET_FILE:IGNDummyPlugins>\"")

# Create a variable for the name of the header which will contain the dummy plugin path.
# This variable gets put in the cache so that it is available at generation time.
foreach(plugin_target
    IGNBadPluginAlign
    IGNBadPluginAPIVersionNew
    IGNBadPluginAPIVersionOld
    IGNBadPluginNoInfo
    IGNBadPluginSize
    IGNDummyPlugins
    IGNFactoryPlugins
    IGNLoadsAnotherPlugin
    IGNTemplatedPlugins)

  target_link_libraries(${plugin_target} PRIVATE
    ${PROJECT_LIBRARY_TARGET_NAME}-register)

endforeach()
