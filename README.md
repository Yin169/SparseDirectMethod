# SparseDirectMethod

High-performance implementation of sparse direct methods for solving large sparse linear systems of equations (Ax = b).

## Overview

This project aims to develop efficient, scalable implementations of sparse direct methods including LU, Cholesky, and QR factorizations. The implementation focuses on:

- Computational efficiency
- Memory optimization
- Numerical stability
- Parallel processing capabilities

## Features

- Support for multiple sparse matrix formats (CSR, CSC, COO)
- Direct factorization methods (LU, Cholesky, QR)
- Matrix reordering algorithms for reduced fill-in
- Shared-memory parallelization (OpenMP)
- Distributed-memory parallelization (MPI)
- Performance profiling tools
- Comprehensive test suite

## Current Implementation Status

### Completed Components
- Basic sparse matrix data structures (CSR format)
- LU factorization implementation
- Performance profiling utilities
- Parallel processing utilities (interface)
- I/O utilities for Matrix Market format
- Example programs
- Unit tests
- Build system (CMake)
- Comprehensive documentation

### Planned Components
- Additional sparse matrix formats (CSC, COO, Block formats)
- Additional factorization methods (Cholesky, QR)
- Matrix reordering algorithms (AMD, Nested Dissection, Reverse Cuthill-McKee)
- Supernodal and multifrontal methods
- GPU acceleration (CUDA)
- Complex number support

## Project Structure

```
src/
├── core/                    # Core data structures and algorithms
│   ├── matrix/             # Sparse matrix representations
│   ├── factorization/      # Direct factorization methods
│   └── solver/             # Linear system solvers
├── utils/                  # Utility functions and helpers
├── parallel/               # Parallel processing components
├── io/                     # Input/output operations
├── performance/            # Profiling and benchmarking tools
├── tests/                  # Unit and integration tests
└── examples/               # Example applications and usage
```

See [project_outline.md](project_outline.md) for detailed project structure and implementation plan.

## Getting Started

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, Intel C++ Compiler 19+)
- CMake 3.12+
- BLAS/LAPACK library
- OpenMP (optional, for shared-memory parallelization)
- MPI (optional, for distributed-memory parallelization)

### Building

```bash
mkdir build
cd build
cmake ..
make
```

### Running Tests

```bash
make test
```

## Documentation

Detailed documentation can be found in the [docs](docs/) directory:

- [API Documentation](docs/api.md)
- [User Guide](docs/user_guide.md)
- [Performance Tuning Guide](docs/performance.md)
- [Build and Run Guide](docs/build_and_run.md)

## Examples

Check out the [examples](examples/) directory for sample programs demonstrating how to use the library.

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct and the process for submitting pull requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- This project is inspired by established libraries like UMFPACK, SuperLU, and MUMPS
- Thanks to the SuiteSparse collection for test matrices
