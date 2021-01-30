cmake_minimum_required(VERSION 2.8.11)

project (sudoku_solver)

include(CTest)

set(CMAKE_CXX_FLAGS
    "-Wall -Werror -pedantic -std=c++2a -ggdb -O2 ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address -O0")

file(GLOB sources "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(FILTER sources EXCLUDE REGEX "main.cpp")
file(GLOB main "${CMAKE_SOURCE_DIR}/src/main.cpp")

add_executable(sudoku_solver ${main} ${sources})

add_custom_target(manual_test COMMAND
        sudoku_solver < ${CMAKE_SOURCE_DIR}/manual_test)
add_dependencies(manual_test sudoku_solver)

add_custom_target(clang_format
        COMMAND clang-format -i
        ${CMAKE_SOURCE_DIR}/test/*.cpp
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

if (BUILD_TESTING)

  find_package(GTest)
  set(GTEST_LINKED_AS_SHARED_LIBRARY 1)

  file(GLOB tests "${CMAKE_SOURCE_DIR}/test/*.cpp")
  add_executable(unit_test ${tests} ${sources})
  target_include_directories(unit_test PRIVATE "${CMAKE_SOURCE_DIR}/src")
  target_link_libraries(unit_test gtest_main gtest)
  add_test(NAME unit_test COMMAND unit_test)

endif()
