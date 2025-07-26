target_sources(SparseDirectMethod 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/linear_solver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/multifrontal_solver.cpp
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/linear_solver.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/multifrontal_solver.hpp
)
