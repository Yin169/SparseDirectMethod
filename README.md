# SparseDirectMethod - Multifrontal Method Implementation

This project implements a multifrontal method for solving sparse linear systems using the Eigen library.

## Overview

The multifrontal method is a direct method for solving sparse linear systems Ax = b. It is particularly efficient for large sparse systems because it decomposes the problem into smaller subproblems (fronts) that can be solved more efficiently.

## Features

- Sparse matrix analysis and pattern recognition
- Multifrontal decomposition approach
- LDL^T factorization for symmetric matrices
- Forward and backward substitution for solving linear systems

## Prerequisites

- C++11 or later
- Eigen3 library
- CMake 3.10 or later

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```cpp
#include "SparseDirectMethod.h"

// Create sparse matrix A
Eigen::SparseMatrix<double> A(n, n);
// ... populate matrix ...

// Create right-hand side vector
Eigen::VectorXd b(n);
// ... populate vector ...

// Create solver
SparseDirectMethod solver;

// Analyze pattern
solver.analyzePattern(A);

// Factorize
solver.factorize(A);

// Solve
Eigen::VectorXd x = solver.solve(b);
```

## Implementation Details

The implementation follows the standard multifrontal method approach:

1. **Symbolic Analysis**: 
   - Analyze the sparsity pattern of the input matrix
   - Construct an elimination tree
   - Build the assembly tree of frontal matrices

2. **Numeric Factorization**:
   - Assemble frontal matrices
   - Perform LDL^T factorization on each front
   - Assemble contributions to parent fronts

3. **Solution Phase**:
   - Forward substitution
   - Backward substitution

## Limitations

This is a simplified implementation for demonstration purposes. A production version would include:

- More sophisticated pivot selection
- Supernodal techniques for better performance
- Parallel processing capabilities
- More advanced fill-in reduction ordering algorithms
- Support for unsymmetric matrices

## References

1. Duff, I. S., & Erisman, A. M. (1983). Direct methods for sparse matrices.
2. Davis, T. A. (2006). Direct methods for sparse linear systems.
3. Liu, J. W. (1992). The multifrontal method for sparse matrix solution: Theory and practice.