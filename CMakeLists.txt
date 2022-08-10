cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(gz-plugin2 VERSION 2.0.0)

#============================================================================
# Find gz-cmake
#============================================================================
find_package(gz-cmake3 REQUIRED)
set(GZ_CMAKE_VER ${gz-cmake3_VERSION_MAJOR})

#============================================================================
# Configure the project
#============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

gz_configure_project(VERSION_SUFFIX pre1)

#============================================================================
# Set project-specific options
#============================================================================

if (UNIX AND NOT APPLE)
  set (EXTRA_TEST_LIB_DEPS stdc++fs)
else()
  set (EXTRA_TEST_LIB_DEPS)
endif()

#============================================================================
# Search for project-specific dependencies
#============================================================================
message(STATUS "\n\n-- ====== Finding Dependencies ======")

#--------------------------------------
# Find libdl
gz_find_package(DL
  REQUIRED_BY loader
  PRIVATE_FOR loader
  PRETTY libdl PURPOSE "Required for loading plugins")


#--------------------------------------
# Find gz-tools
find_program(GZ_TOOLS_PROGRAM gz)
set(GZ_TOOLS_VER 2)

#--------------------------------------
# Find gz-utils
gz_find_package(gz-utils2 REQUIRED COMPONENTS cli)
set(GZ_UTILS_VER ${gz-utils2_VERSION_MAJOR})

#============================================================================
# Configure the build
#============================================================================
gz_configure_build(QUIT_IF_BUILD_ERRORS
  COMPONENTS loader register)


#============================================================================
# Create package information
#============================================================================
gz_create_packages()

#============================================================================
# Configure documentation
#============================================================================
configure_file(${CMAKE_SOURCE_DIR}/api.md.in ${CMAKE_BINARY_DIR}/api.md)
configure_file(${CMAKE_SOURCE_DIR}/tutorials.md.in ${CMAKE_BINARY_DIR}/tutorials.md)

gz_create_docs(
  API_MAINPAGE_MD "${CMAKE_BINARY_DIR}/api.md"
  TUTORIALS_MAINPAGE_MD "${CMAKE_BINARY_DIR}/tutorials.md"
  )

#============================================================================
# Build examples
#============================================================================

if (BUILD_TESTING)
  # Find dependencies only used in examples
  gz_find_package(gz-common5 QUIET)
  set(GZ_COMMON_VER ${gz-common5_VERSION_MAJOR})

  gz_find_package(gz-math7 QUIET)
  set(GZ_MATH_VER ${gz-math7_VERSION_MAJOR})

  gz_build_examples(
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/examples
    BINARY_DIR ${PROJECT_BINARY_DIR}/examples
    COMPONENTS
      loader register
    DEPENDENCIES
      gz-cmake${GZ_CMAKE_VER}
      gz-common${GZ_COMMON_VER}
      gz-math${GZ_MATH_VER}
      gz-utils${GZ_UTILS_VER}
  )
endif()
