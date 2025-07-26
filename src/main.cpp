#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include "supernodal/SupernodalMethod.h"
#include "multifrontal/MultifrontalMethod.h"
#include "multifrontal/ParallelMultifrontalMethod.h"

// Utility function to check if two vectors are close enough (accounting for floating point errors)
bool vectorsEqual(const double* a, const double* b, int n, double tolerance = 1e-6) {
    for (int i = 0; i < n; i++) {
        if (std::abs(a[i] - b[i]) > tolerance) {
            return false;
        }
    }
    return true;
}

// Example program demonstrating the use of supernodal and multifrontal methods
int main() {
    std::cout << "Sparse Direct Methods Implementation" << std::endl;
    std::cout << "====================================" << std::endl;
    
    // Simple test case:
    // Matrix A = [2 1]  Solution x = [1]   Right-hand side b = [3]
    //            [1 2]           [1]                   [3]
    
    std::cout << "\nSimple Test Case:" << std::endl;
    std::cout << "Matrix A = [2 1]" << std::endl;
    std::cout << "           [1 2]" << std::endl;
    std::cout << "Right-hand side b = [3 3]^T" << std::endl;
    std::cout << "Expected solution x = [1 1]^T" << std::endl;
    
    double b_simple[2] = {3, 3};
    double x_simple[2];
    double expected_simple[2] = {1, 1};
    
    // Example 1: Using Multifrontal Method
    std::cout << "\nExample 1: Multifrontal Method (UMFPACK)" << std::endl;
    
    // Matrix in CSC format for [2 1]
    //                          [1 2]
    int n_simple = 2;
    // Column pointers
    int Ap_simple[3] = {0, 2, 4};
    // Row indices
    int Ai_simple[4] = {0, 1,   // Column 0
                        0, 1};  // Column 1
    // Values
    double Ax_simple[4] = {2, 1,   // Column 0
                           1, 2};  // Column 1
    
    // Create and use multifrontal solver
    MultifrontalMethod mf_solver;
    if (mf_solver.setMatrix(n_simple, Ap_simple, Ai_simple, Ax_simple)) {
        std::cout << "Matrix set successfully" << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        if (mf_solver.factorize()) {
            std::cout << "Factorization successful" << std::endl;
            
            if (mf_solver.solve(b_simple, x_simple)) {
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                
                std::cout << "Solution: [";
                for (int i = 0; i < n_simple; i++) {
                    std::cout << x_simple[i];
                    if (i < n_simple - 1) std::cout << ", ";
                }
                std::cout << "]" << std::endl;
                std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;
                
                if (vectorsEqual(x_simple, expected_simple, n_simple)) {
                    std::cout << "Solution matches expected values!" << std::endl;
                } else {
                    std::cout << "Expected: [1, 1]" << std::endl;
                }
            } else {
                std::cout << "Failed to solve the system" << std::endl;
            }
        } else {
            std::cout << "Factorization failed" << std::endl;
        }
    } else {
        std::cout << "Failed to set matrix" << std::endl;
    }
    
    // Example 2: Using Supernodal Method
    std::cout << "\nExample 2: Supernodal Method (CHOLMOD)" << std::endl;
    
    // For CHOLMOD, we need to create a cholmod_sparse structure
    cholmod_common c;
    cholmod_start(&c);
    
    // Create a sparse matrix in CHOLMOD format (upper triangular part only for symmetric matrix)
    cholmod_sparse* A_cholmod = cholmod_allocate_sparse(n_simple, n_simple, 3, true, true, -1, CHOLMOD_REAL, &c);
    
    // Fill the matrix (upper triangular part)
    int* colptr = (int*)A_cholmod->p;
    int* rowind = (int*)A_cholmod->i;
    double* values = (double*)A_cholmod->x;
    
    // Column 0 (rows 0 and 1)
    colptr[0] = 0;
    rowind[0] = 0; values[0] = 2;
    rowind[1] = 1; values[1] = 1;
    
    // Column 1 (row 1 only, since we're storing upper triangular part)
    colptr[1] = 2;
    rowind[2] = 1; values[2] = 2;
    
    // End
    colptr[2] = 3;
    
    // Create and use supernodal solver
    SupernodalMethod sn_solver;
    if (sn_solver.setMatrix(A_cholmod)) {
        std::cout << "Matrix set successfully" << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        if (sn_solver.factorize()) {
            std::cout << "Factorization successful" << std::endl;
            
            if (sn_solver.solve(b_simple, x_simple)) {
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                
                std::cout << "Solution: [";
                for (int i = 0; i < n_simple; i++) {
                    std::cout << x_simple[i];
                    if (i < n_simple - 1) std::cout << ", ";
                }
                std::cout << "]" << std::endl;
                std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;
                
                if (vectorsEqual(x_simple, expected_simple, n_simple)) {
                    std::cout << "Solution matches expected values!" << std::endl;
                } else {
                    std::cout << "Expected: [1, 1]" << std::endl;
                }
            } else {
                std::cout << "Failed to solve the system" << std::endl;
            }
        } else {
            std::cout << "Factorization failed" << std::endl;
        }
    } else {
        std::cout << "Failed to set matrix" << std::endl;
    }
    
    // Example 3: Using Parallel Multifrontal Method
    std::cout << "\nExample 3: Parallel Multifrontal Method (UMFPACK with OpenMP)" << std::endl;
    
    // Create and use parallel multifrontal solver
    ParallelMultifrontalMethod pmf_solver(4); // Use 4 threads
    if (pmf_solver.setMatrix(n_simple, Ap_simple, Ai_simple, Ax_simple)) {
        std::cout << "Matrix set successfully" << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        if (pmf_solver.factorize()) {
            std::cout << "Factorization successful" << std::endl;
            
            if (pmf_solver.solve(b_simple, x_simple)) {
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                
                std::cout << "Solution: [";
                for (int i = 0; i < n_simple; i++) {
                    std::cout << x_simple[i];
                    if (i < n_simple - 1) std::cout << ", ";
                }
                std::cout << "]" << std::endl;
                std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;
                
                if (vectorsEqual(x_simple, expected_simple, n_simple)) {
                    std::cout << "Solution matches expected values!" << std::endl;
                } else {
                    std::cout << "Expected: [1, 1]" << std::endl;
                }
            } else {
                std::cout << "Failed to solve the system" << std::endl;
            }
        } else {
            std::cout << "Factorization failed" << std::endl;
        }
    } else {
        std::cout << "Failed to set matrix" << std::endl;
    }
    
    // Cleanup
    cholmod_free_sparse(&A_cholmod, &c);
    cholmod_finish(&c);
    
    // Demonstrate larger example for parallel processing benefits
    std::cout << "\n\nLarger Example (5x5 matrix) to show parallel processing benefits:" << std::endl;
    std::cout << "Matrix A (5x5), RHS b = [1 2 3 4 5]^T, Expected solution x = A^-1 * b" << std::endl;
    
    // 5x5 matrix:
    // [ 3  1  0  0  1 ]
    // [ 1  4  2  0  0 ]
    // [ 0  2  5  1  0 ]
    // [ 0  0  1  3  1 ]
    // [ 1  0  0  1  3 ]
    
    int n_large = 5;
    double b_large[5] = {1, 2, 3, 4, 5};
    double x_large[5];
    
    // CSC format for the 5x5 matrix
    int Ap_large[6] = {0, 3, 6, 9, 12, 15};
    int Ai_large[15] = {0, 1, 4,  // Column 0
                        1, 2, 4,  // Column 1
                        1, 2, 3,  // Column 2
                        2, 3, 4,  // Column 3
                        0, 3, 4}; // Column 4
    double Ax_large[15] = {3, 1, 1,   // Column 0
                           4, 2, 1,   // Column 1
                           2, 5, 1,   // Column 2
                           1, 3, 1,   // Column 3
                           1, 1, 3};  // Column 4
    
    // Sequential multifrontal method
    MultifrontalMethod mf_large;
    mf_large.setMatrix(n_large, Ap_large, Ai_large, Ax_large);
    
    auto start_seq = std::chrono::high_resolution_clock::now();
    mf_large.factorize();
    mf_large.solve(b_large, x_large);
    auto end_seq = std::chrono::high_resolution_clock::now();
    auto duration_seq = std::chrono::duration_cast<std::chrono::microseconds>(end_seq - start_seq);
    
    std::cout << "Sequential solution: [";
    for (int i = 0; i < n_large; i++) {
        std::cout << x_large[i];
        if (i < n_large - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "Sequential execution time: " << duration_seq.count() << " microseconds" << std::endl;
    
    // Parallel multifrontal method
    ParallelMultifrontalMethod pmf_large(4);
    pmf_large.setMatrix(n_large, Ap_large, Ai_large, Ax_large);
    
    auto start_par = std::chrono::high_resolution_clock::now();
    pmf_large.factorize();
    pmf_large.solve(b_large, x_large);
    auto end_par = std::chrono::high_resolution_clock::now();
    auto duration_par = std::chrono::duration_cast<std::chrono::microseconds>(end_par - start_par);
    
    std::cout << "Parallel solution: [";
    for (int i = 0; i < n_large; i++) {
        std::cout << x_large[i];
        if (i < n_large - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "Parallel execution time: " << duration_par.count() << " microseconds" << std::endl;
    
    if (duration_seq.count() > 0) {
        double speedup = (double)duration_seq.count() / duration_par.count();
        std::cout << "Speedup: " << speedup << "x" << std::endl;
    }
    
    return 0;
}