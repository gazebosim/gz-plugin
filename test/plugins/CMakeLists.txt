add_library(IGNBadPluginAlign         SHARED BadPluginAlign.cc)
add_library(IGNBadPluginAPIVersionNew SHARED BadPluginAPIVersionNew.cc)
add_library(IGNBadPluginAPIVersionOld SHARED BadPluginAPIVersionOld.cc)
add_library(IGNBadPluginSize          SHARED BadPluginSize.cc)
add_library(IGNDummyPlugins SHARED DummyPlugins.cc)

# Create a variable for the name of the header which will contain the dummy plugin path.
# This variable gets put in the cache so that it is available at generation time.
foreach(plugin_target
    IGNBadPluginAlign
    IGNBadPluginAPIVersionNew
    IGNBadPluginAPIVersionOld
    IGNBadPluginSize
    IGNDummyPlugins)

  target_link_libraries(${plugin_target} PRIVATE ${PROJECT_LIBRARY_TARGET_NAME})

endforeach()
