# newest features used: FetchContent v3.11, FetchContent_MakeAvailable v3.14
cmake_minimum_required(VERSION 3.14)

project(safeLibcCall
  VERSION 2.0
  DESCRIPTION "C++ wrapper for glibc calls to throw errors communicated via \
return values and/or errno as std::system_error exceptions"
  LANGUAGES CXX
  )

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(Getcmake_utils)

include(PreventInSourceBuild)

if(NOT COMMAND init_ctest)
  include(InitCTest)
endif()
init_ctest(
  MEMCHECK
  MEMCHECK_FAILS_TEST
  MEMCHECK_GENERATES_SUPPRESSIONS
)

add_subdirectory(src)
add_subdirectory(test)

if(PROJECT_IS_TOP_LEVEL)
  include(AddDoxygen)
  add_doxygen()

  include(AddMkDocs)
  add_mkdocs()
endif()
