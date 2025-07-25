target_sources(SparseDirectMethod 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/csr_matrix.cpp
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/csr_matrix.hpp
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/sparse_matrix.hpp
)