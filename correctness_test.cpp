#include "SparseDirectMethod.h"
#include <iostream>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <chrono>

int main() {
    // Test with a small, well-conditioned matrix to verify correctness
    const int n = 5;
    Eigen::SparseMatrix<double> A(n, n);
    
    // Create a simple symmetric positive definite matrix
    std::vector<Eigen::Triplet<double>> triplets;
    triplets.push_back(Eigen::Triplet<double>(0, 0, 4.0));
    triplets.push_back(Eigen::Triplet<double>(1, 1, 5.0));
    triplets.push_back(Eigen::Triplet<double>(2, 2, 4.0));
    triplets.push_back(Eigen::Triplet<double>(3, 3, 5.0));
    triplets.push_back(Eigen::Triplet<double>(4, 4, 4.0));
    
    // Off-diagonal elements
    triplets.push_back(Eigen::Triplet<double>(0, 1, -1.0));
    triplets.push_back(Eigen::Triplet<double>(1, 0, -1.0));
    triplets.push_back(Eigen::Triplet<double>(1, 2, -1.0));
    triplets.push_back(Eigen::Triplet<double>(2, 1, -1.0));
    triplets.push_back(Eigen::Triplet<double>(2, 3, -1.0));
    triplets.push_back(Eigen::Triplet<double>(3, 2, -1.0));
    triplets.push_back(Eigen::Triplet<double>(3, 4, -1.0));
    triplets.push_back(Eigen::Triplet<double>(4, 3, -1.0));
    
    A.setFromTriplets(triplets.begin(), triplets.end());
    A.makeCompressed();
    
    std::cout << "Testing with a 5x5 matrix:" << std::endl;
    std::cout << "Matrix created with " << A.nonZeros() << " non-zero elements" << std::endl;
    
    // Create a simple right-hand side vector
    Eigen::VectorXd b(n);
    b << 1.0, 2.0, 3.0, 4.0, 5.0;
    
    std::cout << "RHS vector b = " << b.transpose() << std::endl;
    
    // Create and use our sparse direct method
    SparseDirectMethod solver;
    
    try {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::cout << "\nAnalyzing pattern..." << std::endl;
        solver.analyzePattern(A);
        
        auto analyze_end = std::chrono::high_resolution_clock::now();
        auto analyze_duration = std::chrono::duration_cast<std::chrono::microseconds>(analyze_end - start);
        std::cout << "Pattern analysis took " << analyze_duration.count() << " μs" << std::endl;
        
        std::cout << "Factorizing with multifrontal parallel implementation..." << std::endl;
        solver.factorize(A);
        
        auto factorize_end = std::chrono::high_resolution_clock::now();
        auto factorize_duration = std::chrono::duration_cast<std::chrono::microseconds>(factorize_end - analyze_end);
        std::cout << "Parallel factorization took " << factorize_duration.count() << " μs" << std::endl;
        
        std::cout << "Solving..." << std::endl;
        Eigen::VectorXd x = solver.solve(b);
        
        auto solve_end = std::chrono::high_resolution_clock::now();
        auto solve_duration = std::chrono::duration_cast<std::chrono::microseconds>(solve_end - factorize_end);
        std::cout << "Solving took " << solve_duration.count() << " μs" << std::endl;
        
        auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(solve_end - start);
        std::cout << "Total time: " << total_duration.count() << " μs" << std::endl;
        
        // Verify the solution
        Eigen::VectorXd residual = A * x - b;
        std::cout << "\nSolution x = " << x.transpose() << std::endl;
        std::cout << "Residual A*x - b = " << residual.transpose() << std::endl;
        std::cout << "Residual norm: " << residual.norm() << std::endl;
        
        // Compare with Eigen's solution for verification
        std::cout << "\nComparing with Eigen's SimplicialLDLT:" << std::endl;
        Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> eigenSolver(A);
        Eigen::VectorXd eigen_x = eigenSolver.solve(b);
        std::cout << "Eigen solution x = " << eigen_x.transpose() << std::endl;
        Eigen::VectorXd eigen_residual = A * eigen_x - b;
        std::cout << "Eigen residual norm: " << eigen_residual.norm() << std::endl;
        
        std::cout << "\nDifference between solutions: " << (x - eigen_x).norm() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}