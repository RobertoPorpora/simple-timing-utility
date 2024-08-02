# Include the main source directory
include_directories(${CMAKE_SOURCE_DIR})

# Add the test executable
add_executable(test test.c)

# Link the main library to the test executable
target_link_libraries(test STI)
