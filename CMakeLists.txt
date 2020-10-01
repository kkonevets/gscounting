cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(graph_sage_counting VERSION 0.1.0 LANGUAGES CXX)
add_compile_options(-Wall -Wextra -pedantic)
include_directories(include)

file(GLOB SOURCES 
    "src/*.cpp"
    )

add_executable(gsc ${SOURCES})
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
else()
  target_link_libraries(gsc tbb)
endif()

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
    "tests/*.cpp"
)

add_executable(test_gsc ${TEST_SOURCES})

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_link_libraries(test_gsc gtest_main)
else()
  target_link_libraries(test_gsc gtest_main tbb)
endif()

add_test(NAME test_gsc COMMAND test_gsc)

# GOOGLETEST END
