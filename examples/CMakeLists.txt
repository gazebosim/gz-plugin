cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

project(examples)

# Find the ignition-plugin library
find_package(ignition-plugin0 QUIET REQUIRED COMPONENTS all)
find_package(ignition-common2 QUIET)
find_package(ignition-math5 QUIET REQUIRED)

add_subdirectory(plugins)

file(GLOB example_files "*.cc")
list(SORT example_files)

find_package(Boost COMPONENTS program_options)
if(NOT Boost_PROGRAM_OPTIONS_FOUND)
  message(STATUS "Could not find Boost::program_options. Some examples might "
                 "have a reduced set of features available.")
endif()

foreach(example_file ${example_files})

  get_filename_component(example ${example_file} NAME_WE)

  add_executable(${example} ${example_file})

  target_link_libraries(${example}
    PRIVATE
      ignition-plugin0::loader
      ignition-common2::core
      ignition-math5::core
  )

  # This is only needed for the examples that use plugins, but it doesn't hurt
  # to set it for all the examples.
  target_compile_definitions(${example}
    PRIVATE
      "IGN_PLUGIN_EXAMPLES_LIBDIR=\"${PLUGIN_LIBDIR}\"")

  if(Boost_PROGRAM_OPTIONS_FOUND)
    target_link_libraries(${example} PRIVATE ${Boost_PROGRAM_OPTIONS_LIBRARIES})
    target_include_directories(${example} PRIVATE Boost_INCLUDE_DIRS)
    target_compile_definitions(${example} PRIVATE "HAVE_BOOST_PROGRAM_OPTIONS=1")
  endif()

endforeach()

