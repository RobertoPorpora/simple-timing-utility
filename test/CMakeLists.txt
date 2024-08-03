cmake_minimum_required(VERSION 3.10)
project(test_simple_timing_utility)

add_executable(test test.c)

set_target_properties(test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

include_directories(..)
add_subdirectory(../ ../bin)
target_link_libraries(test STI)