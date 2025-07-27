#include "SparseDirectMethod.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <cassert>
#include <functional>

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
    
    // Use parallel factorization
    assembleAndFactorParallel(matrix);
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
    buildEliminationTreeProper(matrix);
}

void SparseDirectMethod::buildAssemblyTree(const SparseMatrix<double>& matrix) {
    buildAssemblyTreeProper(matrix);
}

void SparseDirectMethod::buildEliminationTreeProper(const SparseMatrix<double>& matrix) {
    int n = matrix.rows();
    elimination_tree.clear();
    elimination_tree.resize(n);
    perm_to_elim.resize(n);
    elim_to_perm.resize(n);
    etree_parent.resize(n, -1);
    etree_children.resize(n);
    
    // Simple ordering - in practice, you would use AMD or similar
    for (int i = 0; i < n; i++) {
        perm_to_elim[i] = i;
        elim_to_perm[i] = i;
    }
    
    // Build adjacency structure
    std::vector<std::set<int>> adjacency(n);
    for (int k = 0; k < matrix.outerSize(); ++k) {
        for (SparseMatrix<double>::InnerIterator it(matrix, k); it; ++it) {
            if (it.row() != it.col()) { // Off-diagonal elements
                adjacency[it.row()].insert(it.col());
                adjacency[it.col()].insert(it.row());
            }
        }
    }
    
    // Build elimination tree using maximum transversal
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
        
        // Connect to the unvisited neighbor with the highest index as parent
        if (!unvisited_neighbors.empty()) {
            // Find the neighbor with maximum index
            int parent = *std::max_element(unvisited_neighbors.begin(), unvisited_neighbors.end());
            etree_parent[v] = parent;
            etree_children[parent].push_back(v);
            elimination_tree[parent].push_back(v);
        }
    }
}

void SparseDirectMethod::buildAssemblyTreeProper(const SparseMatrix<double>& matrix) {
    // Create fronts based on the elimination tree
    fronts.clear();
    
    int n = matrix.rows();
    fronts.resize(n); // One front per node in elimination tree
    
    // Create a front for each node in the elimination tree
    for (int i = 0; i < n; i++) {
        fronts[i] = createFrontForNode(i);
        fronts[i]->id = i;
    }
    
    // Set up dependencies based on elimination tree
    for (int i = 0; i < n; i++) {
        // Children depend on parent
        for (int child : etree_children[i]) {
            fronts[child]->dependents.push_back(i);
            fronts[i]->dependencies.push_back(child);
        }
    }
    
    // Root front (the one with no parent)
    int root = -1;
    for (int i = 0; i < n; i++) {
        if (etree_parent[i] == -1) {
            root = i;
            break;
        }
    }
    
    if (root != -1) {
        root_front = fronts[root];
    }
}

std::shared_ptr<SparseDirectMethod::Front> SparseDirectMethod::createFrontForNode(int node) {
    auto front = std::make_shared<Front>();
    front->id = node;
    
    // Get variables for this front
    front->variables = getVariablesForNode(node);
    
    // For this implementation, we'll eliminate the node itself
    front->eliminated_vars.push_back(node);
    
    // Remaining variables are the rest
    for (int var : front->variables) {
        if (var != node) {
            front->remaining_vars.push_back(var);
        }
    }
    
    return front;
}

std::set<int> SparseDirectMethod::getVariablesForNode(int node) {
    std::set<int> vars;
    vars.insert(node); // Always include the node itself
    
    // Include children nodes in the front
    for (int child : etree_children[node]) {
        vars.insert(child);
    }
    
    // Include some coupling from the adjacency structure
    // This is a simplified approach - in practice this would be more sophisticated
    return vars;
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

void SparseDirectMethod::assembleAndFactorParallel(const SparseMatrix<double>& matrix) {
    // Reset processed flags
    for (auto& front : fronts) {
        front->processed = false;
    }
    
    // Process fronts in parallel
    processFrontsInParallel(matrix);
}

void SparseDirectMethod::processFrontsInParallel(const SparseMatrix<double>& matrix) {
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads <= 0) num_threads = 1; // Fallback if hardware_concurrency is not supported
    
    std::vector<std::thread> threads;
    std::atomic<int> completed_fronts(0);
    int total_fronts = 0;
    
    // Count valid fronts
    for (const auto& front : fronts) {
        if (front) total_fronts++;
    }
    
    // Start worker threads
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([this, &matrix, &completed_fronts, total_fronts]() {
            // Each thread processes available fronts
            while (completed_fronts.load() < total_fronts) {
                std::shared_ptr<Front> front_to_process = nullptr;
                int front_index = -1;
                
                {
                    std::unique_lock<std::mutex> lock(fronts_mutex);
                    // Find a front that is ready to be processed (all dependencies met)
                    for (int i = 0; i < (int)fronts.size(); i++) {
                        auto& front = fronts[i];
                        if (front && !front->processed) {
                            bool can_process = true;
                            // Check if all dependencies have been processed
                            for (int dep : front->dependencies) {
                                if (dep < (int)fronts.size() && fronts[dep] && !fronts[dep]->processed) {
                                    can_process = false;
                                    break;
                                }
                            }
                            
                            if (can_process) {
                                front_to_process = front;
                                front_index = i;
                                break;
                            }
                        }
                    }
                    
                    // If no front is ready, wait or exit if all done
                    if (!front_to_process) {
                        bool all_processed = true;
                        for (auto& front : fronts) {
                            if (front && !front->processed) {
                                all_processed = false;
                                break;
                            }
                        }
                        
                        if (all_processed) {
                            break; // All fronts processed
                        }
                        
                        // Wait for a front to be processed
                        cv.wait_for(lock, std::chrono::milliseconds(10));
                        continue;
                    }
                }
                
                // Process the front outside the lock
                if (front_to_process) {
                    processFront(front_to_process, matrix);
                    
                    {
                        std::lock_guard<std::mutex> lock(fronts_mutex);
                        front_to_process->processed = true;
                        completed_fronts.fetch_add(1);
                    }
                    
                    // Notify waiting threads that a front has been processed
                    cv.notify_all();
                }
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void SparseDirectMethod::processFront(std::shared_ptr<Front> front, const SparseMatrix<double>& matrix) {
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
    
    // Add contributions from child fronts
    addChildContributions(front, var_to_idx);
    
    // Factorize the frontal matrix
    factorizeFrontalMatrix(front);
}

void SparseDirectMethod::addChildContributions(std::shared_ptr<Front> front, std::map<int, int>& var_to_idx) {
    // Add contributions from child fronts to the current front
    // In a real implementation, this would aggregate contributions from child fronts
    // For this simplified version, we'll implement a basic version
    
    // For each child in the elimination tree
    int front_node = front->id;
    for (int child_node : etree_children[front_node]) {
        if (child_node < (int)fronts.size() && fronts[child_node]) {
            auto child_front = fronts[child_node];
            // In a complete implementation, we would add the child's contribution
            // to the current front's frontal matrix
            // For now, we'll skip the detailed computation
        }
    }
}

void SparseDirectMethod::assembleAndFactor(const SparseMatrix<double>& matrix) {
    // Sequential version for comparison
    for (auto& front : fronts) {
        if (front) {
            processFront(front, matrix);
        }
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
    // In multifrontal method, we traverse the elimination tree from leaves to root
    
    int n = x.size();
    if (n == 0) return;
    
    // For this simplified implementation, we'll perform a basic forward substitution
    // directly on the original matrix structure
    // In a complete multifrontal implementation, we would use the L factors from each front
    
    // Since we don't store the complete L matrix, we'll skip detailed computation
    // A full implementation would apply L factors from all fronts in bottom-up order
}

void SparseDirectMethod::backwardSubstitution(VectorXd& x) {
    // Apply backward substitution: solve D*L^T * z = y
    // In multifrontal method, we traverse the elimination tree from root to leaves
    
    int n = x.size();
    if (n == 0) return;
    
    // For this simplified implementation, we'll perform a basic backward substitution
    // In a complete multifrontal implementation, we would use the D and L factors from each front
    
    // Since we don't store the complete factorization, we'll skip detailed computation
    // A full implementation would apply D and L^T factors from all fronts in top-down order
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

int SparseDirectMethod::getFrontForVariable(int variable) {
    // In a real implementation, this would map a variable to its front
    // For this simplified version, we just return 0 (the only front)
    return 0;
}