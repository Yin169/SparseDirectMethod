#ifndef SPARSE_DIRECT_METHOD_MULTIFRONTAL_SOLVER_HPP
#define SPARSE_DIRECT_METHOD_MULTIFRONTAL_SOLVER_HPP

#include "linear_solver.hpp"
#include "core/matrix/sparse_matrix.hpp"
#include "core/matrix/dense_matrix.hpp"
#include "core/solver/eliminationTree.hpp"
#include <memory>
#include <vector>
#include <map>

namespace SparseDirectMethod {
namespace Solver {

    /**
     * @brief Data structure representing a frontal matrix
     */
    struct FrontalMatrix {
        std::unique_ptr<Matrix::DenseMatrix> matrix;  // Dense frontal matrix
        std::vector<int> row_indices;                 // Global row indices
        std::vector<int> col_indices;                 // Global column indices
        std::vector<int> contributions;               // Contributing nodes
        std::vector<int> permutations;                // Permutation vector from LU factorization
        int id;                                       // Front ID
        bool is_contributed;                          // Whether this front has contributed to its parent
        bool is_factorized;                           // Whether this front has been factorized
    };

    /**
     * @brief Data structure representing a supernode
     */
    struct Supernode {
        std::vector<int> nodes;                       // Nodes in this supernode
        std::unique_ptr<Matrix::DenseMatrix> matrix;  // Combined frontal matrix for all nodes
        std::vector<int> row_indices;                 // Global row indices
        std::vector<int> col_indices;                 // Global column indices
        std::vector<int> permutations;                // Permutation vector from LU factorization
        bool is_factorized;                           // Whether this supernode has been factorized
    };

    /**
     * @brief Multifrontal solver implementation
     */
    class MultifrontalSolver : public LinearSolver {
    public:
        MultifrontalSolver();
        ~MultifrontalSolver() override = default;

        void setSolverType(SolverType type) override;
        void setMatrix(std::unique_ptr<SparseMatrix> matrix) override;
        void etreeConstr() override;
        void analyzePattern() override;
        void factorize() override;
        void solve(const double* b, double* x) override;
        size_t getIterations() const override;
        double getResidualNorm() const override;

    private:
        /**
         * @brief Build the elimination tree for the matrix
         */
        void buildEliminationTree();

        /**
         * @brief Create frontal matrices for each node in the elimination tree
         */
        void createFrontalMatrices();

        /**
         * @brief Process fronts in post-order
         */
        void processFronts();

        /**
         * @brief Process a single front
         * @param node Node to process
         */
        void processFront(int node);

        /**
         * @brief Assemble contributions from child fronts
         * @param node Node to assemble contributions for
         */
        void assembleContributions(int node);

        /**
         * @brief Factorize a frontal matrix
         * @param front Frontal matrix to factorize
         */
        void factorizeFront(FrontalMatrix& front);

        /**
         * @brief Create supernodes from the elimination tree
         */
        void createSupernodes();

        /**
         * @brief Process supernodes
         */
        void processSupernodes();

        /**
         * @brief Solve using the factorized fronts
         * @param b Right-hand side vector
         * @param x Solution vector
         */
        void solveWithFronts(const double* b, double* x);

        /**
         * @brief Parallel version of solveWithFronts
         * @param b Right-hand side vector
         * @param x Solution vector
         */
        void solveWithFrontsParallel(const double* b, double* x);

        // Data members
        SolverType solver_type_;
        std::unique_ptr<SparseMatrix> matrix_;
        std::unique_ptr<EliminationTree> elimination_tree_;
        std::map<int, FrontalMatrix> frontal_matrices_;
        std::vector<Supernode> supernodes_;
        std::vector<int> post_ordering_;
        size_t iterations_;
        double residual_norm_;
        bool use_supernodal_;
        bool use_parallel_;
    };

    /**
     * @brief Create a multifrontal solver instance
     * @return Unique pointer to the multifrontal solver
     */
    std::unique_ptr<LinearSolver> createMultifrontalSolver();

} // namespace Solver
} // namespace SparseDirectMethod

#endif // SPARSE_DIRECT_METHOD_MULTIFRONTAL_SOLVER_HPP
