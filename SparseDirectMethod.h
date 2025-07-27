#ifndef SPARSEDIRECTMETHOD_H
#define SPARSEDIRECTMETHOD_H

#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>

using namespace Eigen;

class SparseDirectMethod {
private:
    bool pattern_analyzed;
    bool factorization_done;
    int rows, cols;
    
    // For multifrontal method
    struct Front {
        int id;                            // Unique identifier for the front
        std::set<int> variables;           // Variables in this front
        std::vector<int> eliminated_vars;  // Variables to be eliminated
        std::vector<int> remaining_vars;   // Variables remaining after elimination
        MatrixXd F;                        // Frontal matrix
        VectorXd frontal_rhs;              // Right-hand side for this front
        std::vector<std::shared_ptr<Front>> children; // Children fronts
        MatrixXd L;                        // L factor for this front
        VectorXd D;                        // D factor for this front
        std::vector<int> dependencies;     // Fronts that must be processed before this one
        std::vector<int> dependents;       // Fronts that depend on this one
        bool processed = false;            // Whether this front has been processed
        std::mutex mutex;                  // Mutex for thread-safe access
    };
    
    std::shared_ptr<Front> root_front;     // Root of the frontal tree
    std::vector<std::shared_ptr<Front>> fronts; // All fronts
    
    // Symbolic factorization data
    std::vector<std::vector<int>> elimination_tree;
    std::vector<int> perm_to_elim;         // Permutation to elimination ordering
    std::vector<int> elim_to_perm;         // Inverse permutation
    
    // Data structures for front management
    std::vector<std::vector<int>> etree_children; // Children in elimination tree
    std::vector<int> etree_parent;                // Parent in elimination tree
    
    // Numeric factorization data
    std::vector<MatrixXd> L_blocks;        // L factor blocks
    std::vector<VectorXd> D_diag;          // Diagonal blocks
    
    // Parallel processing data
    std::mutex fronts_mutex;
    std::condition_variable cv;
    int pending_fronts;
    
public:
    SparseDirectMethod();
    ~SparseDirectMethod();
    
    void analyzePattern(const SparseMatrix<double>& matrix);
    void factorize(const SparseMatrix<double>& matrix);
    VectorXd solve(const VectorXd& rhs);
    
private:
    // Symbolic analysis methods
    void buildEliminationTree(const SparseMatrix<double>& matrix);
    void buildAssemblyTree(const SparseMatrix<double>& matrix);
    std::shared_ptr<Front> createFront(const std::set<int>& variables);
    std::shared_ptr<Front> createFrontForNode(int node);
    
    // Numeric factorization methods
    void factorizeFrontalMatrix(std::shared_ptr<Front> front);
    void assembleAndFactor(const SparseMatrix<double>& matrix);
    void assembleAndFactorParallel(const SparseMatrix<double>& matrix);
    void processFront(std::shared_ptr<Front> front, const SparseMatrix<double>& matrix);
    void processFrontsInParallel(const SparseMatrix<double>& matrix);
    
    // Solve methods
    void forwardSubstitution(VectorXd& x);
    void backwardSubstitution(VectorXd& x);
    
    // Utility methods
    std::vector<int> computeAmalgamatedSupervariables(const SparseMatrix<double>& matrix);
    
    // Helper methods for parallel processing
    void addChildContributions(std::shared_ptr<Front> front, std::map<int, int>& var_to_idx);
    int getFrontForVariable(int variable);
    
    // New methods for proper multifrontal implementation
    void buildEliminationTreeProper(const SparseMatrix<double>& matrix);
    void buildAssemblyTreeProper(const SparseMatrix<double>& matrix);
    std::set<int> getVariablesForNode(int node);
};

#endif // SPARSEDIRECTMETHOD_H