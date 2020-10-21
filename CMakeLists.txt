cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(graph_sage_counting VERSION 0.1.0 LANGUAGES CXX)
add_compile_options(-Wall -Wextra -pedantic)
include_directories(include /usr/local/include)
link_directories(/usr/local/lib)

file(GLOB SOURCES 
    "src/*.cpp"
    )

# set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=abseil*,bugbrone*,cppcoreguidelines*,modernize*,performance*,readability*)

add_executable(gsc ${SOURCES})
target_link_libraries(gsc tbb)

# GOOGLETEST BEGIN

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

file(GLOB TEST_SOURCES
  "tests/test_io.cpp"
  "src/csr_matrix.cpp"
)

add_executable(test ${TEST_SOURCES})

# if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
target_link_libraries(test gtest_main tbb)

add_test(NAME test COMMAND test)

# GOOGLETEST END
