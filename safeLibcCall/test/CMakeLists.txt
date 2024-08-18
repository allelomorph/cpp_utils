# TBD requires v3.X
# cmake_minimum_required(VERSION 3.10)

# should set _CATCH_VERSION_MAJOR
include(GetCatch2)

add_executable(unit_tests
  safeLibcCall_test.cc
)
target_link_libraries(unit_tests
  PRIVATE
    safeLibcCall
    Catch2::Catch2WithMain
)
target_compile_definitions(unit_tests
  PUBLIC
    _CATCH_VERSION_MAJOR=${_CATCH_VERSION_MAJOR}
)

# see https://github.com/catchorg/Catch2/blob/v3.4.0/docs/cmake-integration.md
# CTest.cmake calls enable_testing(), but it must also be called in project root
include(CTest)
include(Catch)
catch_discover_tests(unit_tests)

add_custom_command(TARGET unit_tests POST_BUILD
  COMMAND ctest -C $<CONFIGURATION> --output-on-failure --verbose
)
