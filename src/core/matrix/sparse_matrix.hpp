#ifndef SPARSE_DIRECT_METHOD_SPARSE_MATRIX_HPP
#define SPARSE_DIRECT_METHOD_SPARSE_MATRIX_HPP

#include <vector>
#include <cstddef>

namespace SparseDirectMethod {

    /**
     * @brief Enumeration of supported sparse matrix formats
     */
    enum class MatrixFormat {
        COO,  // Coordinate format
        CSR,  // Compressed Sparse Row format
        CSC   // Compressed Sparse Column format
    };

    /**
     * @brief Base class for sparse matrix representations
     * 
     * This class provides the interface for different sparse matrix formats
     * and common operations on sparse matrices.
     */
    class SparseMatrix {
    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~SparseMatrix() = default;

        /**
         * @brief Get the number of rows
         * @return Number of rows
         */
        virtual size_t rows() const = 0;

        /**
         * @brief Get the number of columns
         * @return Number of columns
         */
        virtual size_t cols() const = 0;

        /**
         * @brief Get the number of non-zero elements
         * @return Number of non-zero elements
         */
        virtual size_t nnz() const = 0;

        /**
         * @brief Get the matrix format
         * @return Matrix format
         */
        virtual MatrixFormat format() const = 0;

        /**
         * @brief Convert to a different matrix format
         * @param format Target format
         * @return New matrix in the target format
         */
        virtual SparseMatrix* to_format(MatrixFormat format) const = 0;

        /**
         * @brief Matrix-vector multiplication
         * @param x Input vector
         * @param y Output vector (pre-allocated)
         */
        virtual void matvec(const double* x, double* y) const = 0;

        /**
         * @brief Get a deep copy of the matrix
         * @return Pointer to a new matrix instance
         */
        virtual SparseMatrix* clone() const = 0;

    protected:
        /**
         * @brief Protected constructor to prevent direct instantiation
         */
        SparseMatrix() = default;
    };

} // namespace SparseDirectMethod

#endif // SPARSE_DIRECT_METHOD_SPARSE_MATRIX_HPP
