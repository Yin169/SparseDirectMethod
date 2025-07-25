# SparseDirectMethod API Documentation

## Overview

The SparseDirectMethod library provides high-performance implementations of sparse direct methods for solving large sparse linear systems of equations (Ax = b). This documentation covers the main components of the API and how to use them.

## Core Components

### Sparse Matrix Representations

The library supports several sparse matrix formats:

#### SparseMatrix (Base Class)

The `SparseMatrix` class is the base class for all sparse matrix representations.

```cpp
#include "core/matrix/sparse_matrix.hpp"

using namespace SparseDirectMethod;
```

Key methods:
- `rows()` - Get the number of rows
- `cols()` - Get the number of columns
- `nnz()` - Get the number of non-zero elements
- `format()` - Get the matrix format
- `to_format(MatrixFormat format)` - Convert to a different format
- `matvec(const double* x, double* y)` - Matrix-vector multiplication
- `clone()` - Create a deep copy

#### CSRMatrix (Compressed Sparse Row)

The `CSRMatrix` class implements the Compressed Sparse Row format, which is efficient for row-wise operations.

```cpp
#include "core/matrix/csr_matrix.hpp"

// Create a CSR matrix
std::vector<double> values = {1.0, 2.0, 3.0, 4.0, 5.0};
std::vector<size_t> column_indices = {0, 2, 1, 0, 2};
std::vector<size_t> row_pointers = {0, 2, 3, 5};

CSRMatrix matrix(3, 3, 
                 std::move(values),
                 std::move(column_indices),
                 std::move(row_pointers));
```

### Linear Solvers

The library provides several direct solvers for linear systems.

#### LinearSolver Interface

The `LinearSolver` interface provides a unified way to solve linear systems:

```cpp
#include "core/solver/linear_solver.hpp"

using namespace SparseDirectMethod::Solver;

// Create a solver
auto solver = createSolver(SolverType::LU);

// Set the matrix
solver->setMatrix(std::make_unique<CSRMatrix>(matrix));

// Analyze pattern and factorize
solver->analyzePattern();
solver->factorize();

// Solve the system Ax = b
std::vector<double> b = {1.0, 2.0, 3.0};
std::vector<double> x(3, 0.0);
solver->solve(b.data(), x.data());
```

### Factorization Methods

The library implements several matrix factorization methods.

#### LU Factorization

LU factorization with partial pivoting:

```cpp
#include "core/factorization/lu_factorization.hpp"

using namespace SparseDirectMethod::Factorization;

// Compute LU factorization
auto lu = compute_lu(matrix);

// Solve using the factorization
std::vector<double> b = {1.0, 2.0, 3.0};
std::vector<double> x(3, 0.0);
solve_lu(lu, b.data(), x.data());
```

## Utility Functions

### I/O Utilities

Functions for reading and writing matrices and vectors:

```cpp
#include "utils/io_utils.hpp"

using namespace SparseDirectMethod::Utils;

// Read a matrix from Matrix Market format
auto matrix = read_matrix_market("matrix.mtx");

// Write a matrix to Matrix Market format
write_matrix_market(*matrix, "output.mtx");
```

### Performance Profiling

Simple performance profiling utilities:

```cpp
#include "performance/profiler.hpp"

using namespace SparseDirectMethod::Performance;

// Manual timing
Profiler::getInstance().start("matrix_operation");
// ... perform operation ...
Profiler::getInstance().stop("matrix_operation");
Profiler::getInstance().printResults();

// Automatic timing with RAII
{
    ScopedTimer timer("matrix_operation");
    // ... perform operation ...
    // Timer automatically stops when going out of scope
}
```

## Parallel Processing

Utilities for parallel processing:

```cpp
#include "parallel/parallel_utils.hpp"

using namespace SparseDirectMethod::Parallel;

// Set number of threads
setNumThreads(4);

// Parallel for loop
parallelFor(0, n, [&](size_t i) {
    // Process element i
});

// Parallel reduction
double result = parallelReduce(0, n, 0.0,
    [](double a, double b) { return a + b; },  // Reduce operation
    [&](size_t i) { return data[i]; }          // Map operation
);
```

## Example Usage

A complete example showing how to use the library:

```cpp
#include "core/matrix/csr_matrix.hpp"
#include "core/solver/linear_solver.hpp"
#include "utils/io_utils.hpp"
#include "performance/profiler.hpp"

using namespace SparseDirectMethod;
using namespace SparseDirectMethod::Solver;
using namespace SparseDirectMethod::Performance;

int main() {
    // Create a simple 3x3 matrix:
    // [ 2.0  1.0  0.0 ]
    // [ 1.0  2.0  1.0 ]
    // [ 0.0  1.0  2.0 ]
    
    std::vector<double> values = {2.0, 1.0, 1.0, 2.0, 1.0, 1.0, 2.0};
    std::vector<size_t> column_indices = {0, 1, 0, 1, 2, 1, 2};
    std::vector<size_t> row_pointers = {0, 2, 5, 7};
    
    CSRMatrix matrix(3, 3,
                     std::move(values),
                     std::move(column_indices),
                     std::move(row_pointers));
    
    // Create solver
    auto solver = createSolver(SolverType::LU);
    solver->setMatrix(matrix.clone());
    
    // Time the factorization
    {
        ScopedTimer timer("factorization");
        solver->analyzePattern();
        solver->factorize();
    }
    
    // Solve system Ax = b where b = [1, 2, 3]
    std::vector<double> b = {1.0, 2.0, 3.0};
    std::vector<double> x(3, 0.0);
    
    {
        ScopedTimer timer("solve");
        solver->solve(b.data(), x.data());
    }
    
    // Print solution
    std::cout << "Solution: [";
    for (size_t i = 0; i < x.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << x[i];
    }
    std::cout << "]" << std::endl;
    
    // Print timing results
    Profiler::getInstance().printResults();
    
    return 0;
}
```

## Error Handling

The library uses standard C++ exception handling:

```cpp
try {
    // Operations that might throw
    auto matrix = read_matrix_market("nonexistent.mtx");
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

## Performance Considerations

1. **Matrix Format Selection**: Choose the appropriate matrix format for your operations:
   - CSR for row-wise operations
   - CSC for column-wise operations
   - COO for easy matrix construction

2. **Memory Management**: Use `clone()` for deep copies and be aware of object ownership.

3. **Preprocessing**: Call `analyzePattern()` before `factorize()` for optimal performance.

4. **Threading**: Set the number of threads appropriately for your hardware.

5. **Profiling**: Use the built-in profiler to identify performance bottlenecks.
