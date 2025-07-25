cmake_minimum_required(VERSION 3.12)
project(SparseDirectMethod VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add source directories
add_subdirectory(src)

# Add examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Add tests
# if(BUILD_TESTS)
#     enable_testing()
#     add_subdirectory(tests)
# endif()