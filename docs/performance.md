# SparseDirectMethod Performance Tuning Guide

## Introduction

This guide provides recommendations for optimizing the performance of the SparseDirectMethod library. The library is designed for high-performance computing, but achieving optimal performance requires careful consideration of several factors.

## Compiler Optimizations

### Optimization Flags

The library uses `-O3` optimization by default, which enables aggressive optimizations. For debugging, use `-DCMAKE_BUILD_TYPE=Debug`.

Additional compiler flags that may improve performance:

```bash
# For GCC/Clang
cmake -DCMAKE_CXX_FLAGS="-O3 -march=native -ffast-math -funroll-loops" ..

# For Intel Compiler
cmake -DCMAKE_CXX_FLAGS="-O3 -xHost -fast" ..
```

### Profile-Guided Optimization

For critical applications, consider using profile-guided optimization (PGO):

```bash
# GCC
cmake -DCMAKE_CXX_FLAGS="-O3 -fprofile-generate" ..
# Run representative workload
# Recompile with profile data
cmake -DCMAKE_CXX_FLAGS="-O3 -fprofile-use" ..
```

## Memory Management

### Matrix Storage Formats

Choose the appropriate matrix format for your operations:

- **CSR (Compressed Sparse Row)**: Best for row-wise operations, matrix-vector multiplication
- **CSC (Compressed Sparse Column)**: Best for column-wise operations
- **COO (Coordinate)**: Best for matrix construction and modification

Convert matrices to the appropriate format before intensive operations:

```cpp
// Convert to CSR for row-wise operations
auto csr_matrix = matrix->to_format(MatrixFormat::CSR);
```

### Memory Alignment

For best performance with SIMD instructions, ensure data is properly aligned:

```cpp
// Use aligned memory allocation for large vectors
double* aligned_vector = (double*)aligned_alloc(32, size * sizeof(double));
// ... use vector ...
free(aligned_vector);
```

## Parallel Processing

### Thread Configuration

Set the number of threads appropriately for your hardware:

```cpp
#include "parallel/parallel_utils.hpp"

using namespace SparseDirectMethod::Parallel;

// For CPU with N cores
setNumThreads(N);

// For hyperthreaded CPUs, experiment with 1x-2x core count
setNumThreads(N * 2);
```

### Environment Variables

Set OpenMP environment variables for optimal performance:

```bash
export OMP_NUM_THREADS=8
export OMP_PROC_BIND=true
export OMP_PLACES=cores
```

### Load Balancing

For parallel algorithms, ensure work is evenly distributed:

```cpp
// Good: Even distribution
parallelFor(0, n, [&](size_t i) {
    // Similar work per iteration
});

// Avoid: Uneven work distribution
parallelFor(0, n, [&](size_t i) {
    // Varying work per iteration
    // Some iterations much slower than others
});
```

## Algorithmic Optimizations

### Matrix Reordering

Use matrix reordering to reduce fill-in during factorization:

```cpp
// TODO: Implement reordering algorithms
// auto reordered_matrix = reorder_matrix(matrix, AMD_ORDERING);
```

### Iterative Refinement

For increased accuracy, use iterative refinement:

```cpp
// TODO: Implement iterative refinement
// refine_solution(matrix, solver, b, x, iterations);
```

### Mixed Precision

For some applications, mixed precision can improve performance:

```cpp
// TODO: Implement mixed precision solvers
// auto solver = createSolver(SolverType::LU, Precision::MIXED);
```

## Profiling and Benchmarking

### Built-in Profiler

Use the built-in profiler to identify bottlenecks:

```cpp
#include "performance/profiler.hpp"

using namespace SparseDirectMethod::Performance;

// Profile critical sections
{
    ScopedTimer timer("matrix_factorization");
    solver->factorize();
}

// Print results
Profiler::getInstance().printResults();
```

### External Profilers

For detailed analysis, use external profilers:

```bash
# Valgrind with Callgrind
valgrind --tool=callgrind ./sparse_solver matrix.mtx rhs.vec

# Intel VTune
vtune -collect hotspots ./sparse_solver matrix.mtx rhs.vec

# perf (Linux)
perf record ./sparse_solver matrix.mtx rhs.vec
perf report
```

### Benchmarking

Benchmark your code with representative datasets:

```cpp
// Benchmark different matrix sizes
for (size_t n : {1000, 5000, 10000, 50000}) {
    auto matrix = generate_test_matrix(n);
    
    ScopedTimer timer("solve_" + std::to_string(n));
    solver->analyzePattern();
    solver->factorize();
    solver->solve(b.data(), x.data());
}
```

## Hardware Considerations

### Cache Optimization

Structure your data and algorithms to maximize cache efficiency:

```cpp
// Access data in sequential order when possible
for (size_t i = 0; i < matrix.rows(); ++i) {
    // Process row i completely before moving to row i+1
}

// Avoid random memory access patterns
```

### NUMA Awareness

For NUMA systems, consider memory placement:

```bash
# Run with NUMA binding
numactl --cpunodebind=0 --membind=0 ./sparse_solver matrix.mtx rhs.vec
```

### Vectorization

Ensure your code can take advantage of SIMD instructions:

```cpp
// Compiler-friendly loop
for (size_t i = 0; i < n; ++i) {
    // Simple, predictable operations
    y[i] = a * x[i] + y[i];
}
```

## I/O Optimization

### Matrix Market Format

For large matrices, consider binary formats instead of text:

```cpp
// TODO: Implement binary matrix I/O
// auto matrix = read_binary_matrix("matrix.bin");
```

### Memory-Mapped Files

For very large matrices, use memory-mapped files:

```cpp
// TODO: Implement memory-mapped I/O
// auto matrix = read_mapped_matrix("huge_matrix.mtx");
```

## Library-Specific Optimizations

### Solver Selection

Choose the appropriate solver for your matrix type:

```cpp
// For symmetric positive definite matrices
auto solver = createSolver(SolverType::Cholesky);

// For general matrices
auto solver = createSolver(SolverType::LU);

// For least squares problems
auto solver = createSolver(SolverType::QR);
```

### Preconditioning

For iterative solvers (future work), use appropriate preconditioners:

```cpp
// TODO: Implement preconditioned iterative solvers
// solver->setPreconditioner(PreconditionerType::ILU);
```

## Performance Monitoring

### Regular Benchmarking

Regularly benchmark your application to detect performance regressions:

```bash
# Run benchmarks and save results
./run_benchmarks.sh > benchmark_$(date +%Y%m%d).txt
```

### Performance Regression Testing

Integrate performance testing into your CI pipeline:

```cmake
# CMakeLists.txt
add_test(NAME PerformanceTest COMMAND performance_test)
set_tests_properties(PerformanceTest PROPERTIES TIMEOUT 300)
```

## Troubleshooting Performance Issues

### Common Performance Problems

1. **Memory Bandwidth Saturation**
   - Solution: Optimize data access patterns, use blocking

2. **Load Imbalance**
   - Solution: Redistribute work evenly, use dynamic scheduling

3. **Cache Misses**
   - Solution: Improve data locality, use cache-friendly algorithms

4. **Synchronization Overhead**
   - Solution: Reduce critical sections, use lock-free data structures

### Diagnostic Steps

1. Profile the application to identify hotspots
2. Check hardware resource utilization (CPU, memory, I/O)
3. Verify compiler optimizations are enabled
4. Ensure parallelization is effective
5. Review algorithmic complexity

## Best Practices Summary

1. **Compile with optimizations enabled**
2. **Choose appropriate matrix formats for operations**
3. **Set thread count to match hardware**
4. **Use the built-in profiler to identify bottlenecks**
5. **Structure data for cache efficiency**
6. **Regularly benchmark performance**
7. **Consider hardware-specific optimizations**
8. **Profile with representative workloads**

By following these guidelines, you should be able to achieve near-optimal performance with the SparseDirectMethod library.
