# Create library
add_library(SparseDirectMethod 
    main.cpp
)

# Include directories
target_include_directories(SparseDirectMethod 
    PUBLIC ${PROJECT_SOURCE_DIR}
    PUBLIC ${PROJECT_SOURCE_DIR}/src
)

# Link dependencies
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

# Add OpenMP support
option(ENABLE_OPENMP "Enable OpenMP parallelization" ON)
if(ENABLE_OPENMP)
    find_package(OpenMP)
    if(OpenMP_FOUND)
        target_link_libraries(SparseDirectMethod PRIVATE OpenMP::OpenMP_CXX)
    endif()
endif()

# Link BLAS/LAPACK
target_link_libraries(SparseDirectMethod 
    PRIVATE ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES}
)

add_subdirectory(core)
add_subdirectory(utils)
add_subdirectory(performance)