# SparseDirectMethod User Guide

## Introduction

This user guide provides practical instructions on how to use the SparseDirectMethod library to solve sparse linear systems. The library implements high-performance sparse direct methods including LU, Cholesky, and QR factorizations.

## Getting Started

### Installation

Follow the build instructions in [build_and_run.md](build_and_run.md) to compile the library.

### Basic Usage

To use the library in your own project, you need to:

1. Include the necessary headers
2. Link against the `sparse_direct_method` library
3. Use the appropriate classes and functions

Here's a minimal example:

```cpp
#include "core/matrix/csr_matrix.hpp"
#include "core/solver/linear_solver.hpp"

using namespace SparseDirectMethod;
using namespace SparseDirectMethod::Solver;

int main() {
    // Create a sparse matrix (example: 3x3 matrix)
    std::vector<double> values = {2.0, -1.0, -1.0, 2.0, -1.0, -1.0, 2.0};
    std::vector<size_t> column_indices = {0, 1, 0, 1, 2, 1, 2};
    std::vector<size_t> row_pointers = {0, 2, 5, 7};
    
    CSRMatrix matrix(3, 3,
                     std::move(values),
                     std::move(column_indices),
                     std::move(row_pointers));
    
    // Create and configure solver
    auto solver = createSolver(SolverType::LU);
    solver->setMatrix(matrix.clone());
    solver->analyzePattern();
    solver->factorize();
    
    // Solve the system Ax = b
    std::vector<double> b = {1.0, 0.0, 1.0};
    std::vector<double> x(3, 0.0);
    solver->solve(b.data(), x.data());
    
    // Output the solution
    for (size_t i = 0; i < x.size(); ++i) {
        std::cout << "x[" << i << "] = " << x[i] << std::endl;
    }
    
    return 0;
}
```

## Working with Sparse Matrices

### Creating Matrices

The library supports several sparse matrix formats. The most commonly used is CSR (Compressed Sparse Row):

```cpp
// Create a CSR matrix from raw data
std::vector<double> values = { /* non-zero values */ };
std::vector<size_t> column_indices = { /* column indices */ };
std::vector<size_t> row_pointers = { /* row pointers */ };

CSRMatrix matrix(num_rows, num_cols,
                 std::move(values),
                 std::move(column_indices),
                 std::move(row_pointers));
```

### Matrix Operations

Basic matrix operations include:

```cpp
// Matrix-vector multiplication
std::vector<double> x(matrix.cols(), 1.0);
std::vector<double> y(matrix.rows(), 0.0);
matrix.matvec(x.data(), y.data());

// Matrix properties
size_t rows = matrix.rows();
size_t cols = matrix.cols();
size_t nnz = matrix.nnz();
```

## Solving Linear Systems

### Choosing a Solver

The library provides several solver types:

```cpp
// For general matrices
auto lu_solver = createSolver(SolverType::LU);

// For symmetric positive definite matrices
auto cholesky_solver = createSolver(SolverType::Cholesky);

// For least squares problems
auto qr_solver = createSolver(SolverType::QR);
```

### Solver Workflow

The typical workflow for solving a linear system is:

1. Create the solver
2. Set the matrix
3. Analyze the pattern (optional but recommended)
4. Factorize the matrix
5. Solve the system

```cpp
auto solver = createSolver(SolverType::LU);
solver->setMatrix(matrix.clone());

// Preprocessing step (optional but recommended for performance)
solver->analyzePattern();

// Factorize the matrix
solver->factorize();

// Solve one or more systems with the same matrix
solver->solve(b1.data(), x1.data());
solver->solve(b2.data(), x2.data());
```

## Input/Output Operations

### Matrix Market Format

The library supports reading and writing matrices in Matrix Market format:

```cpp
#include "utils/io_utils.hpp"

using namespace SparseDirectMethod::Utils;

// Read a matrix
auto matrix = read_matrix_market("input.mtx");

// Write a matrix
write_matrix_market(*matrix, "output.mtx");
```

### Dense Vectors

For reading and writing dense vectors:

```cpp
// Read a vector
size_t size;
double* vector = read_dense_vector("input.vec", size);

// Write a vector
write_dense_vector(vector, size, "output.vec");

// Don't forget to free the memory
delete[] vector;
```

## Performance Optimization

### Profiling

Use the built-in profiler to identify performance bottlenecks:

```cpp
#include "performance/profiler.hpp"

using namespace SparseDirectMethod::Performance;

// Profile a code section
Profiler::getInstance().start("critical_section");
// ... your code here ...
Profiler::getInstance().stop("critical_section");

// Print results
Profiler::getInstance().printResults();
```

### Parallel Processing

Enable parallel processing by setting the number of threads:

```cpp
#include "parallel/parallel_utils.hpp"

using namespace SparseDirectMethod::Parallel;

// Set the number of threads
setNumThreads(4);

// Use parallel algorithms
parallelFor(0, n, [&](size_t i) {
    // Process element i in parallel
});
```

## Advanced Topics

### Custom Matrix Formats

To implement a custom sparse matrix format:

1. Inherit from `SparseMatrix`
2. Implement all pure virtual methods
3. Add format-specific functionality

```cpp
class CustomMatrix : public SparseMatrix {
public:
    // Implement all required methods
    size_t rows() const override { /* implementation */ }
    size_t cols() const override { /* implementation */ }
    size_t nnz() const override { /* implementation */ }
    MatrixFormat format() const override { /* implementation */ }
    SparseMatrix* to_format(MatrixFormat format) const override { /* implementation */ }
    void matvec(const double* x, double* y) const override { /* implementation */ }
    SparseMatrix* clone() const override { /* implementation */ }
    
    // Add custom methods
    // ...
};
```

### Custom Solvers

To implement a custom solver:

1. Inherit from `LinearSolver`
2. Implement all pure virtual methods

```cpp
class CustomSolver : public LinearSolver {
public:
    void setSolverType(SolverType type) override { /* implementation */ }
    void setMatrix(std::unique_ptr<SparseMatrix> matrix) override { /* implementation */ }
    void analyzePattern() override { /* implementation */ }
    void factorize() override { /* implementation */ }
    void solve(const double* b, double* x) override { /* implementation */ }
    size_t getIterations() const override { /* implementation */ }
    double getResidualNorm() const override { /* implementation */ }
};
```

## Best Practices

1. **Memory Management**: Use `clone()` for deep copies and be aware of object ownership.

2. **Error Handling**: Always check for exceptions when performing I/O operations or matrix operations.

3. **Performance**: 
   - Call `analyzePattern()` before `factorize()` for optimal performance
   - Reuse factorizations when solving multiple systems with the same matrix
   - Choose the appropriate matrix format for your operations

4. **Threading**: Set the number of threads appropriately for your hardware.

5. **Testing**: Use the provided test framework to verify correctness.

## Troubleshooting

### Common Issues

1. **Linker Errors**: Make sure you're linking against the `sparse_direct_method` library.

2. **Runtime Errors**: Check that matrix dimensions are consistent and that sufficient memory is available.

3. **Performance Issues**: 
   - Profile your code to identify bottlenecks
   - Ensure you're using the appropriate matrix format
   - Check that parallel processing is enabled if needed

### Getting Help

If you encounter issues not covered in this guide:

1. Check the API documentation
2. Review the example programs
3. Run the unit tests to verify the library is working correctly
4. File an issue on the project's GitHub repository
