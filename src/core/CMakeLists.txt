# Core library
set(CORE_SOURCES
    SparseDirectMethod.cpp
)

set(CORE_HEADERS
    SparseDirectMethod.h
)

add_library(core ${CORE_SOURCES} ${CORE_HEADERS})
target_link_libraries(core ${SUITESPARSE_LIBRARIES} ${OPENBLAS_LIBRARIES})