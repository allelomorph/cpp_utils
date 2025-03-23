# newest feature PROJECT_IS_TOP_LEVEL v3.21
# cmake_minimum_required(VERSION 3.21)

if(NOT COMMAND set_strict_compile_options)
  include(SetStrictCompileOptions)
endif()

if(NOT COMMAND add_catch2_tests)
  include(AddCatch2Tests)
endif()

set(tests_target unit_tests)
if(NOT PROJECT_IS_TOP_LEVEL)
  set(tests_target ${PROJECT_NAME}_${tests_target})
endif()

add_executable(${tests_target}
  typeName_test.cc
)
target_link_libraries(${tests_target}
  PRIVATE typeName
)
set_target_properties(${tests_target} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)
set_strict_compile_options(${tests_target})

add_catch2_tests(${tests_target}
  MEMCHECK
)
