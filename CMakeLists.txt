cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(graph_sage_counting VERSION 0.1.0 LANGUAGES CXX)
add_compile_options(-Wall -Wextra -pedantic)

add_subdirectory(src)
