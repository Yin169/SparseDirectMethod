#ifndef SPARSE_DIRECT_METHOD_DENSE_MATRIX_HPP
#define SPARSE_DIRECT_METHOD_DENSE_MATRIX_HPP

#include <vector>
#include <cstddef>
#include <memory>

namespace SparseDirectMethod {
namespace Matrix {

    /**
     * @brief Dense matrix class for use in multifrontal method
     */
    class DenseMatrix {
    public:
        /**
         * @brief Constructor for dense matrix
         * @param rows Number of rows
         * @param cols Number of columns
         */
        DenseMatrix(size_t rows, size_t cols);

        /**
         * @brief Copy constructor
         * @param other Matrix to copy
         */
        DenseMatrix(const DenseMatrix& other);

        /**
         * @brief Move constructor
         * @param other Matrix to move
         */
        DenseMatrix(DenseMatrix&& other) noexcept;

        /**
         * @brief Copy assignment operator
         * @param other Matrix to copy
         * @return Reference to this matrix
         */
        DenseMatrix& operator=(const DenseMatrix& other);

        /**
         * @brief Move assignment operator
         * @param other Matrix to move
         * @return Reference to this matrix
         */
        DenseMatrix& operator=(DenseMatrix&& other) noexcept;

        /**
         * @brief Destructor
         */
        ~DenseMatrix() = default;

        /**
         * @brief Get the number of rows
         * @return Number of rows
         */
        size_t rows() const;

        /**
         * @brief Get the number of columns
         * @return Number of columns
         */
        size_t cols() const;

        /**
         * @brief Get element at position (i, j)
         * @param i Row index
         * @param j Column index
         * @return Element value
         */
        double& operator()(size_t i, size_t j);

        /**
         * @brief Get element at position (i, j) (const version)
         * @param i Row index
         * @param j Column index
         * @return Element value
         */
        const double& operator()(size_t i, size_t j) const;

        /**
         * @brief Get raw data pointer
         * @return Pointer to data array
         */
        double* data();

        /**
         * @brief Get raw data pointer (const version)
         * @return Pointer to data array
         */
        const double* data() const;

        /**
         * @brief Fill matrix with zeros
         */
        void zero();

        /**
         * @brief Perform LU factorization with partial pivoting
         * @param permutations Permutation vector (output)
         */
        void lu_factorize(std::vector<int>& permutations);

        /**
         * @brief Solve linear system Ax = b using LU factorization
         * @param permutations Permutation vector from factorization
         * @param b Right-hand side vector
         * @param x Solution vector (output)
         */
        void lu_solve(const std::vector<int>& permutations, const double* b, double* x) const;

        /**
         * @brief Matrix-vector multiplication
         * @param x Input vector
         * @param y Output vector (pre-allocated)
         */
        void matvec(const double* x, double* y) const;

        /**
         * @brief Add a contribution from another matrix
         * @param other Matrix to add
         * @param row_offset Row offset for placement
         * @param col_offset Column offset for placement
         */
        void add_contribution(const DenseMatrix& other, size_t row_offset, size_t col_offset);

        /**
         * @brief Extract a submatrix
         * @param row_start Starting row index
         * @param row_end Ending row index (exclusive)
         * @param col_start Starting column index
         * @param col_end Ending column index (exclusive)
         * @return Extracted submatrix
         */
        DenseMatrix extract_submatrix(size_t row_start, size_t row_end, 
                                     size_t col_start, size_t col_end) const;

        /**
         * @brief Get a deep copy of the matrix
         * @return Pointer to a new matrix instance
         */
        std::unique_ptr<DenseMatrix> clone() const;

    private:
        size_t rows_;              ///< Number of rows
        size_t cols_;              ///< Number of columns
        std::vector<double> data_; ///< Matrix data in row-major order
    };

} // namespace Matrix
} // namespace SparseDirectMethod

#endif // SPARSE_DIRECT_METHOD_DENSE_MATRIX_HPP
