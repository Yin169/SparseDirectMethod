#include "SparseDirectMethod.h"
#include <iostream>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <chrono>
#include <random>

int main() {
    // Test with a larger sparse matrix
    const int n = 1000;  // 1000x1000 matrix
    std::cout << "Creating a " << n << "x" << n << " sparse matrix..." << std::endl;
    
    Eigen::SparseMatrix<double> A(n, n);
    
    // Create a sparse matrix with a pattern similar to FEM matrices
    // Each row will have connections to nearby nodes (like a 2D grid)
    std::vector<Eigen::Triplet<double>> triplets;
    
    // Use a random number generator to create a more realistic sparse matrix
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.1, 10.0);
    
    // Create a band matrix with additional random connections
    for (int i = 0; i < n; i++) {
        // Main diagonal
        triplets.push_back(Eigen::Triplet<double>(i, i, dis(gen) + 10.0));
        
        // Off-diagonal elements (nearby connections)
        if (i > 0) {
            double val = dis(gen);
            triplets.push_back(Eigen::Triplet<double>(i, i-1, val));
            triplets.push_back(Eigen::Triplet<double>(i-1, i, val));
        }
        
        if (i < n-1) {
            double val = dis(gen);
            triplets.push_back(Eigen::Triplet<double>(i, i+1, val));
            triplets.push_back(Eigen::Triplet<double>(i+1, i, val));
        }
        
        if (i > 10) {
            double val = dis(gen) * 0.1;  // Weaker connections
            triplets.push_back(Eigen::Triplet<double>(i, i-10, val));
            triplets.push_back(Eigen::Triplet<double>(i-10, i, val));
        }
        
        if (i < n-10) {
            double val = dis(gen) * 0.1;  // Weaker connections
            triplets.push_back(Eigen::Triplet<double>(i, i+10, val));
            triplets.push_back(Eigen::Triplet<double>(i+10, i, val));
        }
        
        // Some random long-range connections to make it more interesting
        if (i % 50 == 0) {
            for (int j = 0; j < 5; j++) {
                int target = std::uniform_int_distribution<int>(0, n-1)(gen);
                if (std::abs(target - i) > 20 && target != i) {
                    double val = dis(gen) * 0.01;
                    triplets.push_back(Eigen::Triplet<double>(i, target, val));
                    triplets.push_back(Eigen::Triplet<double>(target, i, val));
                }
            }
        }
    }
    
    A.setFromTriplets(triplets.begin(), triplets.end());
    A.makeCompressed();
    
    std::cout << "Matrix created with " << A.nonZeros() << " non-zero elements (" 
              << (100.0 * A.nonZeros() / (double)(n*n)) << "% density)" << std::endl;
    
    // Create right-hand side vector
    Eigen::VectorXd b(n);
    for (int i = 0; i < n; i++) {
        b(i) = dis(gen);
    }
    
    // Create and use our sparse direct method
    SparseDirectMethod solver;
    
    try {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::cout << "Analyzing pattern..." << std::endl;
        solver.analyzePattern(A);
        
        auto analyze_end = std::chrono::high_resolution_clock::now();
        auto analyze_duration = std::chrono::duration_cast<std::chrono::milliseconds>(analyze_end - start);
        std::cout << "Pattern analysis took " << analyze_duration.count() << " ms" << std::endl;
        
        std::cout << "Factorizing..." << std::endl;
        solver.factorize(A);
        
        auto factorize_end = std::chrono::high_resolution_clock::now();
        auto factorize_duration = std::chrono::duration_cast<std::chrono::milliseconds>(factorize_end - analyze_end);
        std::cout << "Factorization took " << factorize_duration.count() << " ms" << std::endl;
        
        std::cout << "Solving..." << std::endl;
        Eigen::VectorXd x = solver.solve(b);
        
        auto solve_end = std::chrono::high_resolution_clock::now();
        auto solve_duration = std::chrono::duration_cast<std::chrono::milliseconds>(solve_end - factorize_end);
        std::cout << "Solving took " << solve_duration.count() << " ms" << std::endl;
        
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(solve_end - start);
        std::cout << "Total time: " << total_duration.count() << " ms" << std::endl;
        
        // Verify the solution
        Eigen::VectorXd residual = A * x - b;
        std::cout << "Residual norm: " << residual.norm() << std::endl;
        
        // Show first 10 elements of solution
        std::cout << "First 10 elements of solution:" << std::endl;
        for (int i = 0; i < std::min(10, n); i++) {
            std::cout << "x[" << i << "] = " << x(i) << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}