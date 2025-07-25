#include "lu_factorization.hpp"
#include "core/matrix/csr_matrix.hpp"
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <cmath>

namespace SparseDirectMethod {
namespace Factorization {

    LUFactorization compute_lu(const SparseMatrix& matrix) {
        // For now, we only support CSR matrices
        if (matrix.format() != MatrixFormat::CSR) {
            throw std::invalid_argument("Only CSR format is currently supported");
        }
        
        // Cast to CSR matrix
        const CSRMatrix* csr_matrix = static_cast<const CSRMatrix*>(&matrix);
        
        // Check if matrix is square
        if (csr_matrix->rows() != csr_matrix->cols()) {
            throw std::invalid_argument("Matrix must be square for LU factorization");
        }
        
        size_t n = csr_matrix->rows();
        
        // Get the matrix data
        const std::vector<double>& values = csr_matrix->get_values();
        const std::vector<size_t>& column_indices = csr_matrix->get_column_indices();
        const std::vector<size_t>& row_pointers = csr_matrix->get_row_pointers();
        
        // Create copies for L and U matrices
        std::vector<double> l_values;
        std::vector<size_t> l_column_indices;
        std::vector<size_t> l_row_pointers(n + 1, 0);
        
        std::vector<double> u_values;
        std::vector<size_t> u_column_indices;
        std::vector<size_t> u_row_pointers(n + 1, 0);
        
        // Initialize permutation vector
        std::vector<int> permutations(n);
        for (size_t i = 0; i < n; ++i) {
            permutations[i] = static_cast<int>(i);
        }
        
        // Simple LU decomposition without pivoting for now
        // This is a simplified implementation and not optimized for performance
        std::vector<std::vector<double>> dense_matrix(n, std::vector<double>(n, 0.0));
        
        // Convert CSR to dense (not efficient, but simple for demonstration)
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = row_pointers[i]; j < row_pointers[i + 1]; ++j) {
                dense_matrix[i][column_indices[j]] = values[j];
            }
        }
        
        // LU decomposition
        for (size_t k = 0; k < n; ++k) {
            // Find pivot (partial pivoting)
            size_t pivot_row = k;
            double max_val = std::abs(dense_matrix[k][k]);
            
            for (size_t i = k + 1; i < n; ++i) {
                if (std::abs(dense_matrix[i][k]) > max_val) {
                    max_val = std::abs(dense_matrix[i][k]);
                    pivot_row = i;
                }
            }
            
            // Swap rows if needed
            if (pivot_row != k) {
                std::swap(dense_matrix[k], dense_matrix[pivot_row]);
                std::swap(permutations[k], permutations[pivot_row]);
            }
            
            // Check for zero pivot
            if (std::abs(dense_matrix[k][k]) < 1e-12) {
                throw std::runtime_error("Matrix is singular or nearly singular");
            }
            
            // Compute L and U entries
            for (size_t i = k + 1; i < n; ++i) {
                dense_matrix[i][k] /= dense_matrix[k][k];
                
                for (size_t j = k + 1; j < n; ++j) {
                    dense_matrix[i][j] -= dense_matrix[i][k] * dense_matrix[k][j];
                }
            }
        }
        
        // Extract L and U matrices
        std::vector<double> l_vals;
        std::vector<size_t> l_cols;
        std::vector<size_t> l_rows(n + 1, 0);
        
        std::vector<double> u_vals;
        std::vector<size_t> u_cols;
        std::vector<size_t> u_rows(n + 1, 0);
        
        size_t l_nnz = 0;
        size_t u_nnz = 0;
        
        for (size_t i = 0; i < n; ++i) {
            l_rows[i] = l_nnz;
            u_rows[i] = u_nnz;
            
            // L matrix (lower triangular with unit diagonal)
            for (size_t j = 0; j < i; ++j) {
                if (std::abs(dense_matrix[i][j]) > 1e-12) {
                    l_vals.push_back(dense_matrix[i][j]);
                    l_cols.push_back(j);
                    l_nnz++;
                }
            }
            // Unit diagonal for L
            l_vals.push_back(1.0);
            l_cols.push_back(i);
            l_nnz++;
            
            // U matrix (upper triangular)
            for (size_t j = i; j < n; ++j) {
                if (std::abs(dense_matrix[i][j]) > 1e-12) {
                    u_vals.push_back(dense_matrix[i][j]);
                    u_cols.push_back(j);
                    u_nnz++;
                }
            }
        }
        
        l_rows[n] = l_nnz;
        u_rows[n] = u_nnz;
        
        // Create L and U matrices
        auto L = std::make_unique<CSRMatrix>(n, n, 
                                           std::move(l_vals),
                                           std::move(l_cols),
                                           std::move(l_rows));
        
        auto U = std::make_unique<CSRMatrix>(n, n,
                                           std::move(u_vals),
                                           std::move(u_cols),
                                           std::move(u_rows));
        
        return LUFactorization{std::move(L), std::move(U), std::move(permutations)};
    }

    void solve_lu(const LUFactorization& lu, const double* b, double* x) {
        size_t n = lu.L->rows();
        
        // Check dimensions
        if (lu.U->cols() != n || lu.L->cols() != n) {
            throw std::invalid_argument("Incompatible matrix dimensions");
        }
        
        // Apply row permutations to b
        std::vector<double> pb(n);
        for (size_t i = 0; i < n; ++i) {
            pb[i] = b[lu.permutations[i]];
        }
        
        // Forward substitution: Ly = Pb
        std::vector<double> y(n, 0.0);
        
        // Cast L to CSR matrix for access to data
        const CSRMatrix* l_csr = static_cast<const CSRMatrix*>(lu.L.get());
        const std::vector<double>& l_values = l_csr->get_values();
        const std::vector<size_t>& l_column_indices = l_csr->get_column_indices();
        const std::vector<size_t>& l_row_pointers = l_csr->get_row_pointers();
        
        for (size_t i = 0; i < n; ++i) {
            y[i] = pb[i];
            for (size_t j = l_row_pointers[i]; j < l_row_pointers[i + 1] - 1; ++j) {
                y[i] -= l_values[j] * y[l_column_indices[j]];
            }
            // Note: we skip the last element in each row because it's the unit diagonal
        }
        
        // Backward substitution: Ux = y
        std::vector<double> x_vec(n, 0.0);
        
        // Cast U to CSR matrix for access to data
        const CSRMatrix* u_csr = static_cast<const CSRMatrix*>(lu.U.get());
        const std::vector<double>& u_values = u_csr->get_values();
        const std::vector<size_t>& u_column_indices = u_csr->get_column_indices();
        const std::vector<size_t>& u_row_pointers = u_csr->get_row_pointers();
        
        for (size_t i = n; i > 0; --i) {
            size_t idx = i - 1;
            x_vec[idx] = y[idx];
            for (size_t j = u_row_pointers[idx] + 1; j < u_row_pointers[idx + 1]; ++j) {
                x_vec[idx] -= u_values[j] * x_vec[u_column_indices[j]];
            }
            x_vec[idx] /= u_values[u_row_pointers[idx]]; // Divide by diagonal element
        }
        
        // Copy result to output array
        for (size_t i = 0; i < n; ++i) {
            x[i] = x_vec[i];
        }
    }

} // namespace Factorization
} // namespace SparseDirectMethod
