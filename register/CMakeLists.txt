
ign_add_component(register INTERFACE)

install(
  DIRECTORY include/
  DESTINATION ${IGN_INCLUDE_INSTALL_DIR_FULL})

# The -fno-gnu-unique flag is needed by plugin libraries that are compiling
# using GCC. It may result in compilation errors for clang or Visual Studio,
# so we wrap it in a generator expression that will only activate when the
# compiler is GCC (a.k.a. GNU).
#
# Ideally, this flag should only be specified as a compiler option for
# libraries that intend to build themselves as plugin libraries, but to get
# that level of granularity, we would need to move the plugin framework out of
# ign-common and make it its own project (or at the very least, its own
# component). For now, we apply this flag to all projects that depend on
# ign-common, since it doesn't seem to have any negative side-effects. In
# fact, the effect of this flag is to disable a custom GCC extension in the
# linker symbols, and we really don't want different compilers producing
# different behavior anyhow.
target_compile_options(${PROJECT_LIBRARY_TARGET_NAME}-register INTERFACE
  $<$<CXX_COMPILER_ID:GNU>:-fno-gnu-unique>
  $<$<CXX_COMPILER_ID:GNU>:-fkeep-inline-functions>)
