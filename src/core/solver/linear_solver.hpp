#ifndef SPARSE_DIRECT_METHOD_LINEAR_SOLVER_HPP
#define SPARSE_DIRECT_METHOD_LINEAR_SOLVER_HPP

#include "core/matrix/sparse_matrix.hpp"
#include <memory>
#include <vector>

namespace SparseDirectMethod {
namespace Solver {

    /**
     * @brief Enumeration of supported solver types
     */
    enum class SolverType {
        LU,        // LU factorization
        Cholesky,  // Cholesky factorization (for symmetric positive definite matrices)
        QR         // QR factorization
    };

    /**
     * @brief Linear solver interface
     */
    class LinearSolver {
    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~LinearSolver() = default;

        /**
         * @brief Set the solver type
         * @param type The solver type to use
         */
        virtual void setSolverType(SolverType type) = 0;

        /**
         * @brief Set the matrix for the linear system
         * @param matrix The coefficient matrix A in Ax = b
         */
        virtual void setMatrix(std::unique_ptr<SparseMatrix> matrix) = 0;

        /**
         * @brief Analyze the matrix structure for preprocessing
         */
        virtual void analyzePattern() = 0;

        /**
         * @brief Factorize the matrix
         */
        virtual void factorize() = 0;

        /**
         * @brief Solve the linear system Ax = b
         * @param b The right-hand side vector
         * @param x Output vector (solution)
         */
        virtual void solve(const double* b, double* x) = 0;

        /**
         * @brief Get the number of iterations (if applicable)
         * @return Number of iterations
         */
        virtual size_t getIterations() const = 0;

        /**
         * @brief Get the residual norm (if applicable)
         * @return Residual norm
         */
        virtual double getResidualNorm() const = 0;
    };

    /**
     * @brief Create a linear solver instance
     * @param type The type of solver to create
     * @return Unique pointer to the linear solver
     */
    std::unique_ptr<LinearSolver> createSolver(SolverType type);

} // namespace Solver
} // namespace SparseDirectMethod

#endif // SPARSE_DIRECT_METHOD_LINEAR_SOLVER_HPP
