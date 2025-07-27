cmake_minimum_required(VERSION 3.10)
project(SparseDirectMethod)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Eigen
find_package(Eigen3 REQUIRED)

# Add executable for the library
add_library(SparseDirectMethod 
    SparseDirectMethod.cpp
)

target_link_libraries(SparseDirectMethod 
    Eigen3::Eigen
)

target_include_directories(SparseDirectMethod PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add executable for the test
add_executable(test_sparse_direct_method 
    test.cpp
)

target_link_libraries(test_sparse_direct_method 
    SparseDirectMethod
)