cmake_minimum_required(VERSION 3.10)
project(test_simple_timing_utility)

add_executable(${PROJECT_NAME} test.c)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

add_subdirectory("${CMAKE_SOURCE_DIR}/.." STI_build)
target_link_libraries(${PROJECT_NAME} STI)

add_subdirectory("${CMAKE_SOURCE_DIR}/lib/Unity" Unity_build)
target_link_libraries(${PROJECT_NAME} unity)
