cmake_minimum_required(VERSION 3.21)
project(thiefwizard)

add_executable(thiefwizard)
target_compile_options(thiefwizard PUBLIC -Wall -Wpedantic -Werror)

add_subdirectory(3rd_party)

add_subdirectory(inc)
add_subdirectory(src)