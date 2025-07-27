#include "SparseDirectMethod.h"
#include <iostream>
#include <Eigen/Sparse>
#include <Eigen/Dense>

int main() {
    // Create a larger test sparse matrix (8x8)
    // This is a more complex example with a wider bandwidth
    // to better test the sparse direct method implementation
    const int n = 8;
    Eigen::SparseMatrix<double> A(n, n);
    
    // Reserve space for non-zero elements (average of 5 non-zeros per row)
    A.reserve(Eigen::VectorXi::Constant(n, 5));
    
    // Fill the matrix with a more complex pattern
    // Main diagonal
    A.insert(0, 0) = 10;
    A.insert(1, 1) = 8;
    A.insert(2, 2) = 9;
    A.insert(3, 3) = 7;
    A.insert(4, 4) = 11;
    A.insert(5, 5) = 6;
    A.insert(6, 6) = 12;
    A.insert(7, 7) = 5;
    
    // Off-diagonal elements
    A.insert(0, 1) = 2;  A.insert(1, 0) = 2;
    A.insert(0, 2) = 3;  A.insert(2, 0) = 3;
    A.insert(1, 2) = 1;  A.insert(2, 1) = 1;
    A.insert(1, 3) = 4;  A.insert(3, 1) = 4;
    A.insert(2, 3) = 2;  A.insert(3, 2) = 2;
    A.insert(2, 4) = 1;  A.insert(4, 2) = 1;
    A.insert(3, 4) = 3;  A.insert(4, 3) = 3;
    A.insert(3, 5) = 2;  A.insert(5, 3) = 2;
    A.insert(4, 5) = 1;  A.insert(5, 4) = 1;
    A.insert(4, 6) = 4;  A.insert(6, 4) = 4;
    A.insert(5, 6) = 3;  A.insert(6, 5) = 3;
    A.insert(5, 7) = 2;  A.insert(7, 5) = 2;
    A.insert(6, 7) = 1;  A.insert(7, 6) = 1;
    
    A.makeCompressed();
    
    // Create right-hand side vector
    Eigen::VectorXd b(n);
    b << 1, 2, 3, 4, 5, 6, 7, 8;
    
    // Create and use our sparse direct method
    SparseDirectMethod solver;
    
    try {
        std::cout << "Analyzing pattern..." << std::endl;
        solver.analyzePattern(A);
        
        std::cout << "Factorizing..." << std::endl;
        solver.factorize(A);
        
        std::cout << "Solving..." << std::endl;
        Eigen::VectorXd x = solver.solve(b);
        
        std::cout << "Solution: " << x.transpose() << std::endl;
        
        // Verify the solution
        Eigen::VectorXd residual = A * x - b;
        std::cout << "Residual norm: " << residual.norm() << std::endl;
        std::cout << "Verification A*x = " << (A * x).transpose() << std::endl;
        std::cout << "RHS vector b = " << b.transpose() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}