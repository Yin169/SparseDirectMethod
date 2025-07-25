#ifndef SPARSE_DIRECT_METHOD_CSR_MATRIX_HPP
#define SPARSE_DIRECT_METHOD_CSR_MATRIX_HPP

#include "sparse_matrix.hpp"
#include <vector>
#include <cstddef>

namespace SparseDirectMethod {

    /**
     * @brief Compressed Sparse Row (CSR) matrix format
     * 
     * The CSR format stores a sparse matrix using three arrays:
     * - values: stores the non-zero values
     * - column_indices: stores the column index of each non-zero value
     * - row_pointers: stores the starting position of each row in the values array
     */
    class CSRMatrix : public SparseMatrix {
    public:
        /**
         * @brief Constructor for CSR matrix
         * @param rows Number of rows
         * @param cols Number of columns
         * @param values Non-zero values
         * @param column_indices Column indices of non-zero values
         * @param row_pointers Row pointers array
         */
        CSRMatrix(size_t rows, size_t cols,
                  std::vector<double>&& values,
                  std::vector<size_t>&& column_indices,
                  std::vector<size_t>&& row_pointers);

        /**
         * @brief Copy constructor
         * @param other Matrix to copy
         */
        CSRMatrix(const CSRMatrix& other);

        /**
         * @brief Move constructor
         * @param other Matrix to move
         */
        CSRMatrix(CSRMatrix&& other) noexcept;

        /**
         * @brief Copy assignment operator
         * @param other Matrix to copy
         * @return Reference to this matrix
         */
        CSRMatrix& operator=(const CSRMatrix& other);

        /**
         * @brief Move assignment operator
         * @param other Matrix to move
         * @return Reference to this matrix
         */
        CSRMatrix& operator=(CSRMatrix&& other) noexcept;

        /**
         * @brief Destructor
         */
        ~CSRMatrix() override = default;

        /**
         * @brief Get the number of rows
         * @return Number of rows
         */
        size_t rows() const override;

        /**
         * @brief Get the number of columns
         * @return Number of columns
         */
        size_t cols() const override;

        /**
         * @brief Get the number of non-zero elements
         * @return Number of non-zero elements
         */
        size_t nnz() const override;

        /**
         * @brief Get the matrix format
         * @return Matrix format (CSR)
         */
        MatrixFormat format() const override;

        /**
         * @brief Convert to a different matrix format
         * @param format Target format
         * @return New matrix in the target format
         */
        SparseMatrix* to_format(MatrixFormat format) const override;

        /**
         * @brief Matrix-vector multiplication
         * @param x Input vector
         * @param y Output vector (pre-allocated)
         */
        void matvec(const double* x, double* y) const override;

        /**
         * @brief Get a deep copy of the matrix
         * @return Pointer to a new matrix instance
         */
        SparseMatrix* clone() const override;

        /**
         * @brief Get the values array
         * @return Reference to values array
         */
        const std::vector<double>& get_values() const;

        /**
         * @brief Get the column indices array
         * @return Reference to column indices array
         */
        const std::vector<size_t>& get_column_indices() const;

        /**
         * @brief Get the row pointers array
         * @return Reference to row pointers array
         */
        const std::vector<size_t>& get_row_pointers() const;

    private:
        size_t rows_;                          ///< Number of rows
        size_t cols_;                          ///< Number of columns
        std::vector<double> values_;           ///< Non-zero values
        std::vector<size_t> column_indices_;   ///< Column indices of non-zero values
        std::vector<size_t> row_pointers_;     ///< Row pointers array
    };

} // namespace SparseDirectMethod

#endif // SPARSE_DIRECT_METHOD_CSR_MATRIX_HPP
