# Supernodal method implementation
set(SUPER_NODE_SOURCES
    SupernodalMethod.cpp
)

set(SUPER_NODE_HEADERS
    SupernodalMethod.h
)

add_library(supernodal ${SUPER_NODE_SOURCES} ${SUPER_NODE_HEADERS})
target_link_libraries(supernodal core ${SUITESPARSE_LIBRARIES} ${OPENBLAS_LIBRARIES})
target_include_directories(supernodal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})