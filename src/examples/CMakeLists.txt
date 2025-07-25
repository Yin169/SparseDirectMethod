# Examples CMakeLists.txt
# This file sets up the examples for the SparseDirectMethod project

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Create example executables
add_executable(basic_usage basic_usage.cpp)

# Link libraries
target_link_libraries(basic_usage sparse_direct_method)

# Add more examples as they are created
# add_executable(example_name example_name.cpp)
# target_link_libraries(example_name sparse_direct_method)
