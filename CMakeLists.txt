# newest features used: FetchContent v3.11, FetchContent_MakeAvailable v3.14
cmake_minimum_required(VERSION 3.14)

project(cpp_utils
  DESCRIPTION "Development resources for C++ projects"
  LANGUAGES CXX
)

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(Getcmake_utils)

include(PreventInSourceBuild)

add_subdirectory(IsStreamable)
add_subdirectory(safeLibcCall)
add_subdirectory(typeName)
add_subdirectory(UniformRandNumGen)
