#ifndef SPARSEDIRECTMETHOD_H
#define SPARSEDIRECTMETHOD_H

#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <vector>
#include <map>
#include <set>
#include <memory>

using namespace Eigen;

class SparseDirectMethod {
private:
    bool pattern_analyzed;
    bool factorization_done;
    int rows, cols;
    
    // For multifrontal method
    struct Front {
        std::set<int> variables;           // Variables in this front
        std::vector<int> eliminated_vars;  // Variables to be eliminated
        std::vector<int> remaining_vars;   // Variables remaining after elimination
        MatrixXd F;                        // Frontal matrix
        VectorXd frontal_rhs;              // Right-hand side for this front
        std::vector<std::shared_ptr<Front>> children; // Children fronts
        MatrixXd L;                        // L factor for this front
        VectorXd D;                        // D factor for this front
    };
    
    std::shared_ptr<Front> root_front;     // Root of the frontal tree
    std::vector<std::shared_ptr<Front>> fronts; // All fronts
    
    // Symbolic factorization data
    std::vector<std::vector<int>> elimination_tree;
    std::vector<int> perm_to_elim;         // Permutation to elimination ordering
    
    // Numeric factorization data
    std::vector<MatrixXd> L_blocks;        // L factor blocks
    std::vector<VectorXd> D_diag;          // Diagonal blocks
    
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
    
    // Numeric factorization methods
    void factorizeFrontalMatrix(std::shared_ptr<Front> front);
    void assembleAndFactor(const SparseMatrix<double>& matrix);
    
    // Solve methods
    void forwardSubstitution(VectorXd& x);
    void backwardSubstitution(VectorXd& x);
    
    // Utility methods
    std::vector<int> computeAmalgamatedSupervariables(const SparseMatrix<double>& matrix);
};

#endif // SPARSEDIRECTMETHOD_H