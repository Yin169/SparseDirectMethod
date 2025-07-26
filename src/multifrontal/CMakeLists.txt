# Multifrontal method implementation
set(MULTIFRONTAL_SOURCES
    MultifrontalMethod.cpp
    ParallelMultifrontalMethod.cpp
)

set(MULTIFRONTAL_HEADERS
    MultifrontalMethod.h
    ParallelMultifrontalMethod.h
)

add_library(multifrontal ${MULTIFRONTAL_SOURCES} ${MULTIFRONTAL_HEADERS})
target_link_libraries(multifrontal core ${SUITESPARSE_LIBRARIES} ${OPENBLAS_LIBRARIES})
target_include_directories(multifrontal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Enable OpenMP for multifrontal library
if(APPLE AND EXISTS "/opt/homebrew/opt/libomp/lib/libomp.dylib")
  target_link_libraries(multifrontal "${OpenMP_omp_LIBRARY}")
  target_compile_options(multifrontal PRIVATE "-Xpreprocessor" "-fopenmp")
else()
  find_package(OpenMP REQUIRED)
  target_link_libraries(multifrontal OpenMP::OpenMP_CXX)
  target_compile_options(multifrontal PRIVATE ${OpenMP_CXX_FLAGS})
endif()