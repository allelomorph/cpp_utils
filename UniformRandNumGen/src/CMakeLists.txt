# add_library(<name> INTERFACE [EXCLUDE_FROM_ALL] <sources>...) requires v3.19
cmake_minimum_required(VERSION 3.19)

add_library(UniformRandNumGen INTERFACE UniformRandIntGen.hh)
target_include_directories(UniformRandNumGen INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}"
)
