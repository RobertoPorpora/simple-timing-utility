cmake_minimum_required(VERSION 3.10)

project(simple_timing_utility)

# Add the main library
add_library(STI STATIC STI.c)

# Include directories
target_include_directories(STI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})