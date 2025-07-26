cmake_minimum_required(VERSION 3.10)
project(SparseDirectMethods)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required libraries
find_package(PkgConfig REQUIRED)

# For macOS, we need to explicitly set OpenMP paths
if(APPLE)
  if(EXISTS "/opt/homebrew/opt/libomp")
    set(OpenMP_C_LIB_NAMES "omp")
    set(OpenMP_CXX_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.dylib")
    set(OPENMP_INCLUDE_DIRS "/opt/homebrew/opt/libomp/include")
    set(OPENMP_LIBRARIES "/opt/homebrew/opt/libomp/lib/libomp.dylib")
  endif()
else()
  find_package(OpenMP REQUIRED)
endif()

# Set PKG_CONFIG_PATH to include SuiteSparse and OpenBLAS pkg-config files
set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/Cellar/suite-sparse/7.11.0/lib/pkgconfig:/opt/homebrew/Cellar/openblas/0.3.30/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

# Try to find SuiteSparse components individually
pkg_check_modules(CHOLMOD REQUIRED cholmod)
pkg_check_modules(UMFPACK REQUIRED umfpack)

# Try to find OpenBLAS
pkg_check_modules(OPENBLAS REQUIRED openblas)

# Combine include directories
set(SUITESPARSE_INCLUDE_DIRS ${CHOLMOD_INCLUDE_DIRS} ${UMFPACK_INCLUDE_DIRS})
list(REMOVE_DUPLICATES SUITESPARSE_INCLUDE_DIRS)

# Include directories
include_directories(${SUITESPARSE_INCLUDE_DIRS} ${OPENBLAS_INCLUDE_DIRS})
if(APPLE AND EXISTS "/opt/homebrew/opt/libomp/include")
  include_directories("/opt/homebrew/opt/libomp/include")
endif()

# Add source directories
add_subdirectory(src/core)
add_subdirectory(src/supernodal)
add_subdirectory(src/multifrontal)

# Create executable
add_executable(sparse_direct_methods src/main.cpp)

# Link with required libraries
target_link_libraries(sparse_direct_methods 
                      core 
                      supernodal 
                      multifrontal
                      ${CHOLMOD_LINK_LIBRARIES} 
                      ${UMFPACK_LINK_LIBRARIES} 
                      ${OPENBLAS_LIBRARIES}
                      "-L/opt/homebrew/Cellar/suite-sparse/7.11.0/lib"
                      "-L/opt/homebrew/Cellar/openblas/0.3.30/lib")

# Link OpenMP
if(APPLE AND EXISTS "/opt/homebrew/opt/libomp/lib/libomp.dylib")
  target_link_libraries(sparse_direct_methods "${OpenMP_omp_LIBRARY}")
  target_compile_options(sparse_direct_methods PRIVATE "-Xpreprocessor" "-fopenmp")
else()
  target_link_libraries(sparse_direct_methods OpenMP::OpenMP_CXX)
  target_compile_options(sparse_direct_methods PRIVATE ${OpenMP_CXX_FLAGS})
endif()