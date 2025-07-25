# Contributing to SparseDirectMethod

Thank you for your interest in contributing to SparseDirectMethod! This document provides guidelines and information to help you contribute effectively.

## Code of Conduct

By participating in this project, you are expected to uphold our Code of Conduct. Please report unacceptable behavior to the project maintainers.

## How to Contribute

### Reporting Bugs

Before reporting a bug, please check if it has already been reported. If not, create a new issue with:

- A clear and descriptive title
- A detailed description of the problem
- Steps to reproduce the issue
- Expected behavior vs. actual behavior
- Information about your environment (OS, compiler, library versions)
- Any relevant code snippets or error messages

### Suggesting Enhancements

To suggest a new feature or enhancement:

- Check if there's already an issue or discussion about it
- Create a new issue with a clear title and detailed description
- Explain why the feature would be useful
- If possible, provide examples of how it would be used

### Code Contributions

#### Getting Started

1. Fork the repository
2. Clone your fork:
   ```bash
   git clone https://github.com/your-username/SparseDirectMethod.git
   ```
3. Create a new branch for your feature or bug fix:
   ```bash
   git checkout -b feature/your-feature-name
   ```

#### Development Workflow

1. Follow the coding standards (see below)
2. Write tests for new functionality
3. Ensure all tests pass:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   make test
   ```
4. Update documentation as needed
5. Commit your changes with clear, descriptive messages
6. Push to your fork
7. Submit a pull request

#### Pull Request Process

1. Ensure your code follows the project's coding standards
2. Include tests for any new functionality
3. Update documentation to reflect your changes
4. Describe your changes in the pull request description
5. Reference any related issues
6. Be prepared to make revisions based on feedback

## Coding Standards

### C++ Standards

- Use C++17 features where appropriate
- Follow modern C++ best practices
- Prefer RAII for resource management
- Use smart pointers instead of raw pointers when possible
- Use const correctness consistently

### Naming Conventions

- Classes: `PascalCase`
- Functions: `snake_case`
- Variables: `snake_case`
- Constants: `kPascalCase` or `UPPER_SNAKE_CASE`
- Namespaces: `PascalCase`

### Code Formatting

The project follows these formatting guidelines:

- Use 4 spaces for indentation (no tabs)
- Limit lines to 100 characters
- Place opening braces on the same line as the statement
- Use spaces around operators
- Follow the existing code style in the project

Example:
```cpp
class ExampleClass {
public:
    ExampleClass(int parameter_value);
    
    void doSomething(const std::vector<double>& input_data);
    
private:
    int member_variable_;
    static const int kDefaultValue = 42;
};
```

### Documentation

- Document all public APIs with Doxygen-style comments
- Write clear, concise comments for complex code sections
- Update README and documentation files when making significant changes

Example:
```cpp
/**
 * @brief Computes the LU factorization of a sparse matrix
 * 
 * This function computes the LU factorization with partial pivoting:
 * PA = LU
 * where P is a permutation matrix, L is a lower triangular matrix
 * and U is an upper triangular matrix.
 * 
 * @param matrix The input sparse matrix
 * @return The LU factorization result
 * @throws std::invalid_argument if the matrix is not square
 */
LUFactorization compute_lu(const SparseMatrix& matrix);
```

### Error Handling

- Use exceptions for error conditions
- Provide meaningful error messages
- Follow the existing error handling patterns in the codebase

### Testing

- Write unit tests for new functionality
- Use Google Test framework
- Follow the existing test structure
- Ensure tests are deterministic and fast
- Test edge cases and error conditions

## Development Environment

### Required Tools

- C++17 compatible compiler
- CMake 3.12+
- Google Test
- BLAS and LAPACK libraries

### Building and Testing

See [docs/build_and_run.md](docs/build_and_run.md) for detailed instructions.

## Code Review Process

All pull requests are reviewed by project maintainers. The review process focuses on:

- Code correctness and quality
- Adherence to coding standards
- Test coverage and quality
- Documentation completeness
- Performance considerations

Reviewers may request changes or clarifications before merging.

## Community

### Communication

- For general questions, use the project's discussion forum
- For bug reports and feature requests, use GitHub issues
- For sensitive security issues, contact the maintainers directly

### Getting Help

If you need help with your contribution:

1. Check the documentation
2. Look at existing code examples
3. Ask questions in the discussion forum
4. Reach out to the maintainers if needed

## License

By contributing to SparseDirectMethod, you agree that your contributions will be licensed under the project's MIT License.
