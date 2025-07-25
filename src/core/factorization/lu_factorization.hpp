#ifndef SPARSE_DIRECT_METHOD_LU_FACTORIZATION_HPP
#define SPARSE_DIRECT_METHOD_LU_FACTORIZATION_HPP

#include "core/matrix/sparse_matrix.hpp"
#include <memory>

namespace SparseDirectMethod {
namespace Factorization {

    /**
     * @brief LU factorization result
     */
    struct LUFactorization {
        std::unique_ptr<SparseMatrix> L;  // Lower triangular matrix
        std::unique_ptr<SparseMatrix> U;  // Upper triangular matrix
        std::vector<int> permutations;     // Row permutation vector
    };

    /**
     * @brief Compute the LU factorization of a sparse matrix
     * 
     * This function computes the LU factorization with partial pivoting:
     * PA = LU
     * where P is a permutation matrix, L is a lower triangular matrix
     * and U is an upper triangular matrix.
     * 
     * @param matrix The input sparse matrix
     * @return The LU factorization result
     */
    LUFactorization compute_lu(const SparseMatrix& matrix);

    /**
     * @brief Solve a linear system using LU factorization
     * 
     * Solves the system Ax = b using a previously computed LU factorization.
     * 
     * @param lu The LU factorization
     * @param b The right-hand side vector
     * @param x Output vector (solution)
     */
    void solve_lu(const LUFactorization& lu, const double* b, double* x);

} // namespace Factorization
} // namespace SparseDirectMethod

#endif // SPARSE_DIRECT_METHOD_LU_FACTORIZATION_HPP
