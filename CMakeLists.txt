cmake_minimum_required(VERSION 2.8)

project (sudoku_solver)

set(CMAKE_CXX_FLAGS "-Wall -Werror -pedantic -std=c++2a -ggdb -O2 \
    ${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Werror -pedantic -std=c++2a -O2 -ggdb \
    -fsanitize=address ${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Werror -pedantic -std=c++2a -O2 \
    ${CMAKE_CXX_FLAGS}")

file(GLOB sources "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(sudoku_solver ${sources})

add_custom_target(test COMMAND sudoku_solver < ${CMAKE_SOURCE_DIR}/test)
add_dependencies(test sudoku_solver)

add_custom_target(clang_format 
        COMMAND clang-format -i
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/src/*.hpp)
