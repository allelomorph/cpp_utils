# newest feature PROJECT_IS_TOP_LEVEL v3.21
# cmake_minimum_required(VERSION 3.21)

# should set Catch2_VERSION_MAJOR
include(GetCatch2)

set(tests_target unit_tests)
if(NOT PROJECT_IS_TOP_LEVEL)
  set(tests_target ${PROJECT_NAME}_${tests_target})
endif()

add_executable(${tests_target}
  typeName_test.cc
)
target_link_libraries(${tests_target}
  PRIVATE
    typeName
    Catch2::Catch2WithMain
)
target_compile_definitions(${tests_target}
  PUBLIC
    _CATCH_VERSION_MAJOR=${Catch2_VERSION_MAJOR}
)

# see https://github.com/catchorg/Catch2/blob/v3.4.0/docs/cmake-integration.md
# CTest.cmake calls enable_testing(), but it must also be called in project root
include(CTest)
include(Catch)
catch_discover_tests(${tests_target})

add_custom_command(TARGET ${tests_target} POST_BUILD
  COMMAND ctest -C $<CONFIGURATION> --output-on-failure --verbose
)
