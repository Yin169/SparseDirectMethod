#include "SparseDirectMethod.h"
#include <iostream>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <chrono>
#include <vector>

// Generate a sparse matrix that mimics a 2D finite element method matrix
Eigen::SparseMatrix<double> generateFEMMatrix(int gridSize) {
    int n = gridSize * gridSize;  // Number of nodes in a gridSize x gridSize mesh
    std::vector<Eigen::Triplet<double>> triplets;
    
    // For each node in the grid
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            int node = i * gridSize + j;
            
            // Self-connection (diagonal)
            triplets.push_back(Eigen::Triplet<double>(node, node, 4.0));
            
            // Connect to neighbors (up, down, left, right)
            if (i > 0) {  // Up
                int neighbor = (i-1) * gridSize + j;
                triplets.push_back(Eigen::Triplet<double>(node, neighbor, -1.0));
                triplets.push_back(Eigen::Triplet<double>(neighbor, node, -1.0));
            }
            
            if (i < gridSize - 1) {  // Down
                int neighbor = (i+1) * gridSize + j;
                triplets.push_back(Eigen::Triplet<double>(node, neighbor, -1.0));
                triplets.push_back(Eigen::Triplet<double>(neighbor, node, -1.0));
            }
            
            if (j > 0) {  // Left
                int neighbor = i * gridSize + (j-1);
                triplets.push_back(Eigen::Triplet<double>(node, neighbor, -1.0));
                triplets.push_back(Eigen::Triplet<double>(neighbor, node, -1.0));
            }
            
            if (j < gridSize - 1) {  // Right
                int neighbor = i * gridSize + (j+1);
                triplets.push_back(Eigen::Triplet<double>(node, neighbor, -1.0));
                triplets.push_back(Eigen::Triplet<double>(neighbor, node, -1.0));
            }
        }
    }
    
    Eigen::SparseMatrix<double> A(n, n);
    A.setFromTriplets(triplets.begin(), triplets.end());
    A.makeCompressed();
    
    return A;
}

int main() {
    // Test with a FEM-like sparse matrix
    const int gridSize = 40;  // 40x40 grid = 1600 nodes
    const int n = gridSize * gridSize;
    std::cout << "Creating a FEM-like " << n << "x" << n << " sparse matrix..." << std::endl;
    
    Eigen::SparseMatrix<double> A = generateFEMMatrix(gridSize);
    
    std::cout << "Matrix created with " << A.nonZeros() << " non-zero elements (" 
              << (100.0 * A.nonZeros() / (double)(n*n)) << "% density)" << std::endl;
    
    // Create right-hand side vector
    Eigen::VectorXd b = Eigen::VectorXd::Constant(n, 1.0);
    
    // Create and use our sparse direct method
    SparseDirectMethod solver;
    
    try {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::cout << "Analyzing pattern..." << std::endl;
        solver.analyzePattern(A);
        
        auto analyze_end = std::chrono::high_resolution_clock::now();
        auto analyze_duration = std::chrono::duration_cast<std::chrono::milliseconds>(analyze_end - start);
        std::cout << "Pattern analysis took " << analyze_duration.count() << " ms" << std::endl;
        
        std::cout << "Factorizing with parallel implementation..." << std::endl;
        solver.factorize(A);
        
        auto factorize_end = std::chrono::high_resolution_clock::now();
        auto factorize_duration = std::chrono::duration_cast<std::chrono::milliseconds>(factorize_end - analyze_end);
        std::cout << "Parallel factorization took " << factorize_duration.count() << " ms" << std::endl;
        
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
        
        std::cout << "\nNote: In this simplified implementation, the parallelization is not fully effective\n"
                  << "because we're still using a single front. A complete implementation would create\n"
                  << "multiple fronts based on the elimination tree for better parallelization." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}