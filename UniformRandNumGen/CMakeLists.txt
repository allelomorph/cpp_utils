# newest features used: FetchContent v3.11, FetchContent_MakeAvailable v3.14
cmake_minimum_required(VERSION 3.14)

project(UniformRandNumGen
  DESCRIPTION "Simple class for random real number generation using Marsenne Twister."
  LANGUAGES CXX
  )

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(Getcmake_utils)

include(PreventInSourceBuild)

add_subdirectory(src)
