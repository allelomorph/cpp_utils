# add_library(<name> INTERFACE [EXCLUDE_FROM_ALL] <sources>...) requires v3.19
cmake_minimum_required(VERSION 3.19)

add_library(typeName INTERFACE typeName.hh)
target_include_directories(typeName INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}"
)
