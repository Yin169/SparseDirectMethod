cmake_minimum_required(VERSION 3.12)
project(SparseDirectMethod VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")
endif()

# Find required libraries
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

# Optional OpenMP support
option(USE_OPENMP "Use OpenMP for shared-memory parallelization" ON)
if(USE_OPENMP)
    find_package(OpenMP REQUIRED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Optional MPI support
option(USE_MPI "Use MPI for distributed-memory parallelization" OFF)
if(USE_MPI)
    find_package(MPI REQUIRED)
endif()

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/core/matrix/csr_matrix.cpp
    src/core/factorization/lu_factorization.cpp
    src/core/solver/linear_solver.cpp
    src/parallel/parallel_utils.cpp
    src/performance/profiler.cpp
    src/utils/io_utils.cpp
)

set(HEADERS
    src/core/matrix/sparse_matrix.hpp
    src/core/matrix/csr_matrix.hpp
    src/core/factorization/lu_factorization.hpp
    src/core/solver/linear_solver.hpp
    src/parallel/parallel_utils.hpp
    src/performance/profiler.hpp
    src/utils/io_utils.hpp
)

# Main library
add_library(sparse_direct_method ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(sparse_direct_method ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})
if(USE_OPENMP)
    target_link_libraries(sparse_direct_method OpenMP::OpenMP_CXX)
endif()
if(USE_MPI)
    target_link_libraries(sparse_direct_method MPI::MPI_CXX)
endif()

# Main executable
add_executable(sparse_solver src/main.cpp)
target_link_libraries(sparse_solver sparse_direct_method)

# Examples
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(src/examples)
endif()

# Testing
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/tests)
endif()

# Installation
install(TARGETS sparse_direct_method sparse_solver
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY src/
        DESTINATION include/sparse_direct_method
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
