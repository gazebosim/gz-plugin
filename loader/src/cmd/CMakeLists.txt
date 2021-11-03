add_library(ign STATIC ign.cc)
target_include_directories(ign PUBLIC ${CMAKE_SOURCE_DIR}/loader/include)
target_link_libraries(ign PUBLIC
  ${PROJECT_LIBRARY_TARGET_NAME}
)

set(plugin_executable ign-plugin)
add_executable(${plugin_executable} plugin_main.cc)
target_link_libraries(${plugin_executable}
  ign
  ignition-utils${IGN_UTILS_VER}::cli
  ${loader}
)

install(
  TARGETS
  ${plugin_executable}
  DESTINATION
  ${IGN_LIB_INSTALL_DIR}/ignition/${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}/
)

#===============================================================================
# Generate the ruby script for internal testing.
# Note that the major version of the library is included in the name.
# Ex: cmdplugin1.rb
set(cmd_script_generated_test "${CMAKE_BINARY_DIR}/test/lib/ruby/ignition/cmd${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}.rb")
set(cmd_script_configured_test "${cmd_script_generated_test}.configured")

# Set the library_location variable to the full path of the library file within
# the build directory.
set(plugin_exe_location "$<TARGET_FILE:${plugin_executable}>")

configure_file(
  "cmd${IGN_DESIGNATION}.rb.in"
  "${cmd_script_configured_test}"
  @ONLY)

file(GENERATE
  OUTPUT "${cmd_script_generated_test}"
  INPUT  "${cmd_script_configured_test}")


#===============================================================================
# Used for the installed version.
# Generate the ruby script that gets installed.
# Note that the major version of the library is included in the name.
# Ex: cmdplugin1.rb
set(cmd_script_generated "${CMAKE_CURRENT_BINARY_DIR}/cmd${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}.rb")
set(cmd_script_configured "${cmd_script_generated}.configured")

# Set the library_location variable to the relative path to the library file
# within the install directory structure.
set(plugin_exe_location "../../../${CMAKE_INSTALL_LIBDIR}/ignition/${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}/$<TARGET_FILE_NAME:${plugin_executable}>")

configure_file(
  "cmd${IGN_DESIGNATION}.rb.in"
  "${cmd_script_configured}"
  @ONLY)

file(GENERATE
  OUTPUT "${cmd_script_generated}"
  INPUT  "${cmd_script_configured}")

# Install the ruby command line library in an unversioned location.
install(FILES ${cmd_script_generated} DESTINATION lib/ruby/ignition)
