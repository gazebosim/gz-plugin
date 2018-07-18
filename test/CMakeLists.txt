
# Build gtest
add_library(gtest STATIC gtest/src/gtest-all.cc)
add_library(gtest_main STATIC gtest/src/gtest_main.cc)
target_link_libraries(gtest_main gtest)
target_include_directories(gtest
  PUBLIC "${CMAKE_CURRENT_LIST_DIR}/gtest/include"
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/gtest")

add_subdirectory(performance)
add_subdirectory(integration)
add_subdirectory(static_assertions)
add_subdirectory(plugins)
