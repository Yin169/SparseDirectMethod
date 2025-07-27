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

# Add executable for the large scale test
add_executable(large_test_sparse_direct_method
    large_test.cpp
)

target_link_libraries(large_test_sparse_direct_method
    SparseDirectMethod
)

# Add executable for the FEM-like test
add_executable(fem_test_sparse_direct_method
    fem_test.cpp
)

target_link_libraries(fem_test_sparse_direct_method
    SparseDirectMethod
)

# Add executable for the parallel test
add_executable(parallel_test_sparse_direct_method
    parallel_test.cpp
)

target_link_libraries(parallel_test_sparse_direct_method
    SparseDirectMethod
)

# Add executable for the multifrontal test
add_executable(multifrontal_test_sparse_direct_method
    multifrontal_test.cpp
)

target_link_libraries(multifrontal_test_sparse_direct_method
    SparseDirectMethod
)

# Add executable for the correctness test
add_executable(correctness_test_sparse_direct_method
    correctness_test.cpp
)

target_link_libraries(correctness_test_sparse_direct_method
    SparseDirectMethod
)

# Add executable for the demo test
add_executable(demo_test_sparse_direct_method
    demo_test.cpp
)

target_link_libraries(demo_test_sparse_direct_method
    SparseDirectMethod
)

# Add executable for the correct sparse solver
add_executable(correct_sparse_solver
    correct_sparse_solver.cpp
)

target_link_libraries(correct_sparse_solver
    Eigen3::Eigen
)

# Add executable for the final correct demo
add_executable(final_correct_demo
    final_correct_demo.cpp
)

target_link_libraries(final_correct_demo
    Eigen3::Eigen
)