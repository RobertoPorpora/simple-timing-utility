cmake_minimum_required(VERSION 3.10)

project(simple_timing_utility)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add the main library
add_library(STI STATIC STI.c)

# Include directories
target_include_directories(STI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Option to build tests
option(BUILD_TEST "Build test" OFF)

# Include the test directory if tests are to be built
if(BUILD_TEST)
    add_subdirectory(test)
endif()
