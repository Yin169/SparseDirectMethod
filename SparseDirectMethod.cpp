#include "SparseDirectMethod.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <cassert>

SparseDirectMethod::SparseDirectMethod() 
    : pattern_analyzed(false), factorization_done(false), rows(0), cols(0) {
}

SparseDirectMethod::~SparseDirectMethod() {
}

void SparseDirectMethod::analyzePattern(const SparseMatrix<double>& matrix) {
    // Validate input matrix
    if (matrix.rows() <= 0 || matrix.cols() <= 0) {
        throw std::invalid_argument("Matrix dimensions must be positive");
    }
    
    if (matrix.rows() != matrix.cols()) {
        throw std::invalid_argument("Matrix must be square");
    }
    
    // Store matrix dimensions for later use
    this->rows = matrix.rows();
    this->cols = matrix.cols();
    
    // Build the elimination tree and assembly tree
    buildEliminationTree(matrix);
    buildAssemblyTree(matrix);
    
    pattern_analyzed = true;
}

void SparseDirectMethod::factorize(const SparseMatrix<double>& matrix) {
    if (!pattern_analyzed) {
        throw std::runtime_error("Pattern must be analyzed before factorization");
    }
    
    if (matrix.rows() != rows || matrix.cols() != cols) {
        throw std::invalid_argument("Matrix dimensions don't match the analyzed pattern");
    }
    
    assembleAndFactor(matrix);
    factorization_done = true;
}

VectorXd SparseDirectMethod::solve(const VectorXd& rhs) {
    if (!factorization_done) {
        throw std::runtime_error("Factorization must be done before solving");
    }
    
    if (rhs.size() != rows) {
        throw std::invalid_argument("RHS vector size doesn't match matrix dimensions");
    }
    
    VectorXd x = rhs;
    
    // Forward substitution
    forwardSubstitution(x);
    
    // Backward substitution
    backwardSubstitution(x);
    
    return x;
}

void SparseDirectMethod::buildEliminationTree(const SparseMatrix<double>& matrix) {
    // This is a simplified version - a full implementation would use more sophisticated algorithms
    // like approximate minimum degree (AMD) or nested dissection for ordering
    
    int n = matrix.rows();
    elimination_tree.resize(n);
    perm_to_elim.resize(n);
    
    // Simple ordering for now - in practice, you would use AMD or similar
    for (int i = 0; i < n; i++) {
        perm_to_elim[i] = i;
    }
    
    // Build elimination tree based on matrix structure
    std::vector<std::set<int>> adjacency(n);
    
    // Create adjacency structure
    for (int k = 0; k < matrix.outerSize(); ++k) {
        for (SparseMatrix<double>::InnerIterator it(matrix, k); it; ++it) {
            if (it.row() != it.col()) { // Off-diagonal elements
                adjacency[it.row()].insert(it.col());
                adjacency[it.col()].insert(it.row());
            }
        }
    }
    
    // Simple elimination tree construction
    std::vector<int> parent(n, -1);
    std::vector<bool> visited(n, false);
    
    for (int i = 0; i < n; i++) {
        int v = perm_to_elim[i]; // Eliminate variable in order
        visited[v] = true;
        
        // Find all unvisited neighbors
        std::set<int> unvisited_neighbors;
        for (int neighbor : adjacency[v]) {
            if (!visited[neighbor]) {
                unvisited_neighbors.insert(neighbor);
            }
        }
        
        // Connect to one of the unvisited neighbors as parent
        if (!unvisited_neighbors.empty()) {
            int p = *unvisited_neighbors.begin();
            parent[v] = p;
            elimination_tree[p].push_back(v);
        }
    }
}

void SparseDirectMethod::buildAssemblyTree(const SparseMatrix<double>& matrix) {
    // Create fronts based on the elimination tree
    fronts.clear();
    
    // For a complete implementation, we would traverse the elimination tree
    // and create fronts based on the multifrontal algorithm
    
    // Simplified approach: create one front for now
    std::set<int> all_vars;
    for (int i = 0; i < matrix.rows(); i++) {
        all_vars.insert(i);
    }
    
    root_front = createFront(all_vars);
    fronts.push_back(root_front);
}

std::shared_ptr<SparseDirectMethod::Front> SparseDirectMethod::createFront(const std::set<int>& variables) {
    auto front = std::make_shared<Front>();
    front->variables = variables;
    
    // In a full implementation, we would determine which variables to eliminate
    // and which to keep based on the multifrontal algorithm
    
    // For now, just separate into eliminated and remaining
    int count = 0;
    for (int var : variables) {
        if (count < (int)variables.size() / 2) {
            front->eliminated_vars.push_back(var);
        } else {
            front->remaining_vars.push_back(var);
        }
        count++;
    }
    
    return front;
}

void SparseDirectMethod::assembleAndFactor(const SparseMatrix<double>& matrix) {
    // Assemble and factor each front
    for (auto& front : fronts) {
        // Create the frontal matrix from the original matrix
        int front_size = front->variables.size();
        front->F.resize(front_size, front_size);
        front->F.setZero();
        
        // Map variables to indices in the front
        std::map<int, int> var_to_idx;
        int idx = 0;
        for (int var : front->variables) {
            var_to_idx[var] = idx++;
        }
        
        // Fill the frontal matrix with entries from the original matrix
        for (int k = 0; k < matrix.outerSize(); ++k) {
            for (SparseMatrix<double>::InnerIterator it(matrix, k); it; ++it) {
                int row = it.row();
                int col = it.col();
                
                // Check if both row and column are in this front
                if (var_to_idx.find(row) != var_to_idx.end() && 
                    var_to_idx.find(col) != var_to_idx.end()) {
                    int front_row = var_to_idx[row];
                    int front_col = var_to_idx[col];
                    front->F(front_row, front_col) = it.value();
                }
            }
        }
        
        // Factorize the frontal matrix
        factorizeFrontalMatrix(front);
    }
}

void SparseDirectMethod::factorizeFrontalMatrix(std::shared_ptr<Front> front) {
    // Perform LDL^T factorization on the frontal matrix
    // This is a simplified version - a full implementation would be more complex
    
    int n = front->F.rows();
    if (n == 0) return;
    
    // Simplified LDL^T decomposition
    MatrixXd L = MatrixXd::Identity(n, n);
    VectorXd D = VectorXd::Zero(n);
    
    for (int i = 0; i < n; i++) {
        double d = front->F(i, i);
        for (int k = 0; k < i; k++) {
            d -= L(i, k) * L(i, k) * D(k);
        }
        D(i) = d;
        
        for (int j = i + 1; j < n; j++) {
            double sum = front->F(j, i);
            for (int k = 0; k < i; k++) {
                sum -= L(j, k) * L(i, k) * D(k);
            }
            L(j, i) = sum / D(i);
        }
    }
    
    // Store the factors
    // In a full implementation, we would store these more efficiently
    front->L = L;
    front->D = D;
}

void SparseDirectMethod::forwardSubstitution(VectorXd& x) {
    // Apply forward substitution: solve L * y = x
    // This is a simplified version for the demo
    
    // For a complete multifrontal implementation, we would traverse the assembly tree
    // from root to leaves and apply forward substitution at each front
    
    // Since we only have one front in this simplified implementation:
    if (!fronts.empty() && fronts[0]) {
        auto front = fronts[0];
        int n = front->L.rows();
        
        // Simple forward substitution for lower triangular matrix
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < i; j++) {
                x(i) -= front->L(i, j) * x(j);
            }
            // No division needed since L has 1s on diagonal
        }
    }
}

void SparseDirectMethod::backwardSubstitution(VectorXd& x) {
    // Apply backward substitution: solve D*L^T * z = y
    // This is a simplified version for the demo
    
    // For a complete multifrontal implementation, we would traverse the assembly tree
    // from leaves to root and apply backward substitution at each front
    
    // Since we only have one front in this simplified implementation:
    if (!fronts.empty() && fronts[0]) {
        auto front = fronts[0];
        int n = front->L.rows();
        
        // First solve D * w = y
        for (int i = 0; i < n; i++) {
            if (std::abs(front->D(i)) > 1e-12) {  // Check for non-zero diagonal
                x(i) /= front->D(i);
            }
        }
        
        // Then solve L^T * z = w
        // Simple backward substitution for upper triangular matrix (L^T)
        for (int i = n-1; i >= 0; i--) {
            for (int j = i+1; j < n; j++) {
                x(i) -= front->L(j, i) * x(j); // Using L(j,i) as L^T(i,j)
            }
        }
    }
}

std::vector<int> SparseDirectMethod::computeAmalgamatedSupervariables(const SparseMatrix<double>& matrix) {
    // Compute supervariables for amalgamation
    // This is used to group columns with identical structure
    
    std::vector<int> supervariables(matrix.cols(), 0);
    
    // Simple implementation - in practice, this would be more sophisticated
    for (int i = 0; i < matrix.cols(); i++) {
        supervariables[i] = i; // No amalgamation in this simple version
    }
    
    return supervariables;
}