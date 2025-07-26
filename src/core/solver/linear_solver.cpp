#include "linear_solver.hpp"
#include "multifrontal_solver.hpp"
#include "core/factorization/lu_factorization.hpp"
#include "core/matrix/csr_matrix.hpp"
#include <stdexcept>
#include <memory>

namespace SparseDirectMethod {
namespace Solver {

    // Forward declaration of concrete solver implementations
    class LUSolver;
    /**
     * @brief Concrete implementation of LinearSolver using LU factorization
     */
    class LUSolver : public LinearSolver {
    public:
        LUSolver() : solver_type_(SolverType::LU), iterations_(0), residual_norm_(0.0) {}
        
        void setSolverType(SolverType type) override {
            solver_type_ = type;
        }
        
        void setMatrix(std::unique_ptr<SparseMatrix> matrix) override {
            matrix_ = std::move(matrix);
        }
        
        void etreeConstr() override {
            // For LU solver, no special elimination tree construction needed
        }
        
        void analyzePattern() override {
            // For LU solver, pattern analysis is done during factorization
            // This is a placeholder for more complex solvers that need pattern analysis
        }
        
        void factorize() override {
            if (!matrix_) {
                throw std::runtime_error("Matrix not set");
            }
            
            if (solver_type_ == SolverType::LU) {
                lu_factorization_ = std::make_unique<Factorization::LUFactorization>(
                    Factorization::compute_lu(*matrix_)
                );
            } else {
                throw std::runtime_error("Unsupported solver type");
            }
        }
        
        void solve(const double* b, double* x) override {
            if (!lu_factorization_) {
                throw std::runtime_error("Factorization not computed");
            }
            
            if (solver_type_ == SolverType::LU) {
                Factorization::solve_lu(*lu_factorization_, b, x);
            } else {
                throw std::runtime_error("Unsupported solver type");
            }
            
            // Update statistics
            iterations_ = 1; // Direct solvers typically converge in one step
            residual_norm_ = 0.0; // Exact solution (in theory)
        }
        
        size_t getIterations() const override {
            return iterations_;
        }
        
        double getResidualNorm() const override {
            return residual_norm_;
        }
        
    private:
        SolverType solver_type_;
        std::unique_ptr<SparseMatrix> matrix_;
        std::unique_ptr<Factorization::LUFactorization> lu_factorization_;
        size_t iterations_;
        double residual_norm_;
    };

    /**
     * @brief Factory function to create linear solvers
     * @param type The type of solver to create
     * @return Unique pointer to the linear solver
     */
    std::unique_ptr<LinearSolver> createSolver(SolverType type) {
        switch (type) {
            case SolverType::LU:
                return std::make_unique<LUSolver>();
            case SolverType::Cholesky:
                throw std::runtime_error("Cholesky solver not implemented yet");
            case SolverType::QR:
                throw std::runtime_error("QR solver not implemented yet");
            case SolverType::Multifrontal:
                return createMultifrontalSolver();
            default:
                throw std::invalid_argument("Unknown solver type");
        }
    }

} // namespace Solver
} // namespace SparseDirectMethod
