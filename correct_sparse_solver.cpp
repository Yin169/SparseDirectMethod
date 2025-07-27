#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace Eigen;

class CorrectSparseSolver {
private:
    bool analyzed;
    bool factorized;
    int n;
    
    // Symbolic analysis data
    std::vector<std::vector<int>> elimination_tree;
    std::vector<int> permutation;
    std::vector<int> inverse_permutation;
    std::vector<std::vector<int>> etree_children;
    std::vector<int> etree_parent;
    
    // Numeric factorization data
    std::vector<std::vector<int>> L_indices;
    std::vector<std::vector<double>> L_values;
    std::vector<double> D_values;
    
public:
    CorrectSparseSolver() : analyzed(false), factorized(false), n(0) {}
    
    void analyzePattern(const SparseMatrix<double>& matrix) {
        if (matrix.rows() != matrix.cols()) {
            throw std::invalid_argument("Matrix must be square");
        }
        
        n = matrix.rows();
        
        // Initialize data structures
        elimination_tree.clear();
        elimination_tree.resize(n);
        permutation.resize(n);
        inverse_permutation.resize(n);
        etree_children.resize(n);
        etree_parent.resize(n, -1);
        L_indices.resize(n);
        L_values.resize(n);
        D_values.resize(n);
        
        // Simple natural ordering (in practice, use AMD or similar)
        for (int i = 0; i < n; i++) {
            permutation[i] = i;
            inverse_permutation[i] = i;
        }
        
        // Build elimination tree
        buildEliminationTree(matrix);
        
        analyzed = true;
    }
    
    void factorize(const SparseMatrix<double>& matrix) {
        if (!analyzed) {
            throw std::runtime_error("Pattern must be analyzed first");
        }
        
        if (matrix.rows() != n || matrix.cols() != n) {
            throw std::invalid_argument("Matrix dimensions don't match");
        }
        
        // Perform numerical factorization
        numericalFactorization(matrix);
        
        factorized = true;
    }
    
    VectorXd solve(const VectorXd& rhs) {
        if (!factorized) {
            throw std::runtime_error("Factorization must be done first");
        }
        
        if (rhs.size() != n) {
            throw std::invalid_argument("RHS size doesn't match");
        }
        
        // Forward substitution: solve L * y = b
        VectorXd y = rhs;
        forwardSubstitution(y);
        
        // Diagonal scaling: solve D * z = y
        for (int i = 0; i < n; i++) {
            if (std::abs(D_values[i]) > 1e-12) {
                y[i] /= D_values[i];
            }
        }
        
        // Backward substitution: solve L^T * x = z
        VectorXd x = y;
        backwardSubstitution(x);
        
        return x;
    }
    
private:
    void buildEliminationTree(const SparseMatrix<double>& matrix) {
        // Build adjacency structure
        std::vector<std::set<int>> adjacency(n);
        for (int k = 0; k < matrix.outerSize(); ++k) {
            for (SparseMatrix<double>::InnerIterator it(matrix, k); it; ++it) {
                if (it.row() != it.col()) {
                    adjacency[it.row()].insert(it.col());
                    adjacency[it.col()].insert(it.row());
                }
            }
        }
        
        // Simple elimination tree construction
        std::vector<bool> visited(n, false);
        
        for (int i = 0; i < n; i++) {
            int v = permutation[i];
            visited[v] = true;
            
            // Find unvisited neighbors
            std::set<int> unvisited_neighbors;
            for (int neighbor : adjacency[v]) {
                if (!visited[neighbor]) {
                    unvisited_neighbors.insert(neighbor);
                }
            }
            
            // Connect to neighbor with highest index
            if (!unvisited_neighbors.empty()) {
                int parent = *std::max_element(unvisited_neighbors.begin(), unvisited_neighbors.end());
                etree_parent[v] = parent;
                etree_children[parent].push_back(v);
                elimination_tree[parent].push_back(v);
            }
        }
    }
    
    void numericalFactorization(const SparseMatrix<double>& matrix) {
        // Create a dense representation for working
        std::vector<std::map<int, double>> dense_matrix(n);
        
        // Fill initial matrix
        for (int k = 0; k < matrix.outerSize(); ++k) {
            for (SparseMatrix<double>::InnerIterator it(matrix, k); it; ++it) {
                dense_matrix[it.row()][it.col()] = it.value();
            }
        }
        
        // Perform Cholesky-like factorization in elimination order
        for (int i = 0; i < n; i++) {
            int v = permutation[i]; // Current variable to eliminate
            
            // Compute diagonal element
            double diag = dense_matrix[v][v];
            for (const auto& pair : dense_matrix[v]) {
                int col = pair.first;
                if (col < v) {
                    double val = pair.second;
                    diag -= val * val * D_values[col]; // L[vi] * L[vi] * D[i]
                }
            }
            
            D_values[v] = diag;
            
            // Compute column elements
            std::map<int, double> column_values;
            for (const auto& pair : dense_matrix[v]) {
                int row = pair.first;
                if (row > v) {
                    column_values[row] = pair.second;
                }
            }
            
            // Apply previous contributions
            for (const auto& pair : dense_matrix[v]) {
                int col = pair.first;
                if (col < v) {
                    double L_vi = pair.second;
                    if (std::abs(D_values[col]) > 1e-12) {
                        L_vi /= D_values[col]; // L[vi] = F_vi / D[i]
                    }
                    for (size_t j = 0; j < L_indices[col].size(); j++) {
                        int inner_row = L_indices[col][j];
                        if (inner_row > v) {
                            double inner_val = L_values[col][j];
                            column_values[inner_row] -= L_vi * inner_val * D_values[col];
                        }
                    }
                }
            }
            
            // Store L factors
            for (const auto& pair : column_values) {
                int row = pair.first;
                double val = pair.second;
                if (std::abs(D_values[v]) > 1e-12) {
                    double L_val = val / D_values[v];
                    L_indices[row].push_back(v);
                    L_values[row].push_back(L_val);
                }
            }
        }
    }
    
    void forwardSubstitution(VectorXd& x) {
        // Solve L * y = x
        for (int i = 0; i < n; i++) {
            int v = permutation[i];
            for (size_t j = 0; j < L_indices[v].size(); j++) {
                int col = L_indices[v][j];
                double val = L_values[v][j];
                x[v] -= val * x[col];
            }
        }
    }
    
    void backwardSubstitution(VectorXd& x) {
        // Solve L^T * z = y
        for (int i = n-1; i >= 0; i--) {
            int v = permutation[i];
            for (size_t j = 0; j < L_indices[v].size(); j++) {
                int col = L_indices[v][j];
                double val = L_values[v][j];
                x[col] -= val * x[v];
            }
        }
    }
};

// Test the correct implementation
int main() {
    std::cout << "Testing Correct Sparse Solver Implementation" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    // Create a test matrix
    const int n = 5;
    Eigen::SparseMatrix<double> A(n, n);
    
    std::vector<Eigen::Triplet<double>> triplets;
    triplets.push_back(Eigen::Triplet<double>(0, 0, 4.0));
    triplets.push_back(Eigen::Triplet<double>(1, 1, 5.0));
    triplets.push_back(Eigen::Triplet<double>(2, 2, 4.0));
    triplets.push_back(Eigen::Triplet<double>(3, 3, 5.0));
    triplets.push_back(Eigen::Triplet<double>(4, 4, 4.0));
    
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
    
    std::cout << "Created a " << n << "x" << n << " test matrix" << std::endl;
    
    // Create RHS vector
    Eigen::VectorXd b(n);
    b << 1.0, 2.0, 3.0, 4.0, 5.0;
    
    std::cout << "RHS vector b = " << b.transpose() << std::endl;
    
    try {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Test our implementation
        CorrectSparseSolver solver;
        std::cout << "\nAnalyzing pattern..." << std::endl;
        solver.analyzePattern(A);
        
        auto analyze_end = std::chrono::high_resolution_clock::now();
        auto analyze_duration = std::chrono::duration_cast<std::chrono::microseconds>(analyze_end - start);
        std::cout << "Pattern analysis took " << analyze_duration.count() << " μs" << std::endl;
        
        std::cout << "Factorizing..." << std::endl;
        solver.factorize(A);
        
        auto factorize_end = std::chrono::high_resolution_clock::now();
        auto factorize_duration = std::chrono::duration_cast<std::chrono::microseconds>(factorize_end - analyze_end);
        std::cout << "Factorization took " << factorize_duration.count() << " μs" << std::endl;
        
        std::cout << "Solving..." << std::endl;
        Eigen::VectorXd x = solver.solve(b);
        
        auto solve_end = std::chrono::high_resolution_clock::now();
        auto solve_duration = std::chrono::duration_cast<std::chrono::microseconds>(solve_end - factorize_end);
        std::cout << "Solving took " << solve_duration.count() << " μs" << std::endl;
        
        auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(solve_end - start);
        std::cout << "Total time: " << total_duration.count() << " μs" << std::endl;
        
        // Verify solution
        Eigen::VectorXd residual = A * x - b;
        std::cout << "\nSolution x = " << x.transpose() << std::endl;
        std::cout << "Residual A*x - b = " << residual.transpose() << std::endl;
        std::cout << "Residual norm: " << residual.norm() << std::endl;
        
        // Compare with Eigen's solution
        std::cout << "\nComparing with Eigen's SimplicialLDLT:" << std::endl;
        Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> eigenSolver(A);
        Eigen::VectorXd eigen_x = eigenSolver.solve(b);
        std::cout << "Eigen solution x = " << eigen_x.transpose() << std::endl;
        Eigen::VectorXd eigen_residual = A * eigen_x - b;
        std::cout << "Eigen residual norm: " << eigen_residual.norm() << std::endl;
        
        std::cout << "\nDifference between solutions: " << (x - eigen_x).norm() << std::endl;
        
        // Check if solution is correct (residual should be very small)
        if (residual.norm() < 1e-10) {
            std::cout << "\n✓ Solution is numerically correct!" << std::endl;
        } else {
            std::cout << "\n✗ Solution is not accurate enough." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}