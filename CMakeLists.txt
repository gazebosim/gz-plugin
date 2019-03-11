cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(ignition-plugin1 VERSION 1.0.0)

#============================================================================
# Find ignition-cmake
#============================================================================
find_package(ignition-cmake2 REQUIRED)
set(IGN_CMAKE_VER ${ignition-cmake2_VERSION_MAJOR})

#============================================================================
# Configure the project
#============================================================================
ign_configure_project()


#============================================================================
# Set project-specific options
#============================================================================

# No project-specific options yet for ign-plugin



#============================================================================
# Search for project-specific dependencies
#============================================================================
message(STATUS "\n\n-- ====== Finding Dependencies ======")

#--------------------------------------
# Find libdl
ign_find_package(DL
  REQUIRED_BY loader
  PRIVATE_FOR loader
  PRETTY libdl PURPOSE "Required for loading plugins")


#============================================================================
# Configure the build
#============================================================================
ign_configure_build(QUIT_IF_BUILD_ERRORS
  COMPONENTS loader register)


#============================================================================
# Create package information
#============================================================================
ign_create_packages()

#============================================================================
# Configure documentation
#============================================================================
configure_file(${CMAKE_SOURCE_DIR}/api.md.in ${CMAKE_BINARY_DIR}/api.md)
configure_file(${CMAKE_SOURCE_DIR}/tutorials.md.in ${CMAKE_BINARY_DIR}/tutorials.md)

ign_create_docs(
  API_MAINPAGE_MD "${CMAKE_BINARY_DIR}/api.md"
  TUTORIALS_MAINPAGE_MD "${CMAKE_BINARY_DIR}/tutorials.md"
  )

