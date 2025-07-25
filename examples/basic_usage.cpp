#include "core/matrix/csr_matrix.hpp"
#include <iostream>
#include <vector>

using namespace SparseDirectMethod;

/**
 * @brief Example demonstrating basic usage of the CSR matrix
 */
int main() {
    std::cout << "SparseDirectMethod Basic Usage Example" << std::endl;
    std::cout << "======================================" << std::endl;
    
    // Create a simple 4x4 matrix:
    // [ 1.0  0.0  2.0  0.0 ]
    // [ 0.0  3.0  0.0  4.0 ]
    // [ 5.0  0.0  0.0  6.0 ]
    // [ 0.0  7.0  0.0  8.0 ]
    
    size_t rows = 4;
    size_t cols = 4;
    
    std::vector<double> values = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    std::vector<size_t> column_indices = {0, 2, 1, 3, 0, 3, 1, 3};
    std::vector<size_t> row_pointers = {0, 2, 4, 6, 8};
    
    // Create the CSR matrix
    CSRMatrix matrix(rows, cols,
                     std::move(values),
                     std::move(column_indices),
                     std::move(row_pointers));
    
    // Display matrix information
    std::cout << "Matrix dimensions: " << matrix.rows() << "x" << matrix.cols() << std::endl;
    std::cout << "Number of non-zero elements: " << matrix.nnz() << std::endl;
    std::cout << "Matrix format: CSR" << std::endl;
    
    // Perform matrix-vector multiplication
    std::vector<double> x = {1.0, 2.0, 3.0, 4.0};
    std::vector<double> y(matrix.rows(), 0.0);
    
    matrix.matvec(x.data(), y.data());
    
    // Display results
    std::cout << "\nMatrix-vector multiplication result:" << std::endl;
    std::cout << "Input vector x: [";
    for (size_t i = 0; i < x.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << x[i];
    }
    std::cout << "]" << std::endl;
    
    std::cout << "Output vector y = Ax: [";
    for (size_t i = 0; i < y.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << y[i];
    }
    std::cout << "]" << std::endl;
    
    // Expected result:
    // Row 0: 1.0*1.0 + 0.0*2.0 + 2.0*3.0 + 0.0*4.0 = 7.0
    // Row 1: 0.0*1.0 + 3.0*2.0 + 0.0*3.0 + 4.0*4.0 = 22.0
    // Row 2: 5.0*1.0 + 0.0*2.0 + 0.0*3.0 + 6.0*4.0 = 29.0
    // Row 3: 0.0*1.0 + 7.0*2.0 + 0.0*3.0 + 8.0*4.0 = 46.0
    
    std::cout << "\nExpected result: [7.0, 22.0, 29.0, 46.0]" << std::endl;
    
    return 0;
}
