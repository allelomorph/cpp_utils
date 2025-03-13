# newest feature PROJECT_IS_TOP_LEVEL v3.21
# cmake_minimum_required(VERSION 3.21)

# should set Catch2_VERSION_MAJOR
include(GetCatch2)

if(NOT COMMAND set_strict_compile_options)
  include(SetStrictCompileOptions)
endif()

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
set_target_properties(${tests_target} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
#  POSITION_INDEPENDENT_CODE ON
  )
set_strict_compile_options(${tests_target})
target_compile_definitions(${tests_target}
  PUBLIC
    _CATCH_VERSION_MAJOR=${Catch2_VERSION_MAJOR}
)

set(MEMORYCHECK_COMMAND_OPTIONS
    "-q --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=50 \
--error-exitcode=255 --gen-suppressions=all"
)
set(MEMORYCHECK_SUPPRESSIONS_FILE
#  ${CMAKE_CURRENT_LIST_DIR}/.supp
)

# see https://github.com/catchorg/Catch2/blob/v3.4.0/docs/cmake-integration.md
# CTest.cmake calls enable_testing(), but it must also be called in project root
include(CTest)
include(Catch)
catch_discover_tests(${tests_target}
  PROPERTIES SKIP_RETURN_CODE 4
)

add_custom_command(TARGET ${tests_target} POST_BUILD
  COMMAND
    "ctest" "-C" "$<CONFIGURATION>"
    "--test-action" "memcheck"
    # "--tests-regex" ""
    "--output-on-failure"
    "--verbose"
  WORKING_DIRECTORY
    ${PROJECT_BINARY_DIR}
)
