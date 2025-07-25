# Building and Running the SparseDirectMethod Project

## Prerequisites

Before building the project, ensure you have the following dependencies installed:

- C++17 compatible compiler (GCC 7+, Clang 5+, Intel C++ Compiler 19+)
- CMake 3.12+
- BLAS library (OpenBLAS, Intel MKL, or similar)
- LAPACK library
- Google Test (for unit tests)
- OpenMP (optional, for shared-memory parallelization)
- MPI (optional, for distributed-memory parallelization)

## Building the Project

### Basic Build

1. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

2. Configure the project with CMake:
   ```bash
   cmake ..
   ```

3. Build the project:
   ```bash
   make
   ```

### Advanced Build Options

The project supports several CMake options:

- `USE_OPENMP` (default: ON) - Enable OpenMP support
- `USE_MPI` (default: OFF) - Enable MPI support
- `BUILD_EXAMPLES` (default: ON) - Build example programs
- `BUILD_TESTS` (default: ON) - Build unit tests

To customize these options, pass them to CMake:

```bash
cmake -DUSE_OPENMP=OFF -DBUILD_TESTS=OFF ..
```

### Installing Dependencies

#### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install build-essential cmake libblas-dev liblapack-dev libgtest-dev libomp-dev
```

#### macOS

```bash
brew install cmake openblas lapack googletest libomp
```

#### CentOS/RHEL

```bash
sudo yum install gcc gcc-c++ make cmake blas-devel lapack-devel gtest-devel
```

## Running the Project

### Main Executable

After building, the main executable `sparse_solver` will be available in the build directory:

```bash
./sparse_solver [options] <matrix_file> <rhs_file>
```

### Examples

The project includes several example programs that demonstrate various features:

```bash
# Run the basic usage example
./src/examples/basic_usage
```

### Tests

To run the unit tests:

```bash
make test
```

Or run the test executable directly:

```bash
./src/tests/sparse_direct_method_tests
```

## Project Structure

The project is organized as follows:

```
SparseDirectMethod/
├── src/                    # Source code
│   ├── core/              # Core algorithms and data structures
│   ├── utils/             # Utility functions
│   ├── parallel/          # Parallel processing components
│   ├── io/                # Input/output operations
│   ├── performance/       # Profiling and benchmarking tools
│   ├── tests/             # Unit tests
│   └── examples/          # Example programs
├── docs/                  # Documentation
├── build/                 # Build directory (created during build)
├── CMakeLists.txt         # Main CMake configuration
├── README.md              # Project overview
└── project_outline.md     # Detailed project outline
```

## Troubleshooting

### Common Build Issues

1. **Missing BLAS/LAPACK libraries**
   - Ensure the libraries are installed
   - Specify library paths explicitly if needed:
     ```bash
     cmake -DBLAS_LIBRARIES=/path/to/blas -DLAPACK_LIBRARIES=/path/to/lapack ..
     ```

2. **Google Test not found**
   - Install Google Test or disable tests:
     ```bash
     cmake -DBUILD_TESTS=OFF ..
     ```

3. **Compiler not supporting C++17**
   - Use a newer compiler or specify a different compiler:
     ```bash
     cmake -DCMAKE_CXX_COMPILER=/path/to/compiler ..
     ```

### Performance Tuning

1. **Optimization flags**
   - The project uses `-O3` optimization by default
   - For debugging, use `-DCMAKE_BUILD_TYPE=Debug`

2. **Threading**
   - Set the number of threads with environment variables:
     ```bash
     export OMP_NUM_THREADS=4
     ```

3. **Memory allocation**
   - For large problems, ensure sufficient memory is available
   - Consider using memory-mapped files for very large matrices

## Contributing

To contribute to the project:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

Please follow the coding standards and ensure all tests pass before submitting a pull request.
