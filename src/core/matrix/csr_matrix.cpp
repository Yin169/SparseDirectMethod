#include "csr_matrix.hpp"
#include <algorithm>
#include <stdexcept>

namespace SparseDirectMethod {

    CSRMatrix::CSRMatrix(size_t rows, size_t cols,
                         std::vector<double>&& values,
                         std::vector<size_t>&& column_indices,
                         std::vector<size_t>&& row_pointers)
        : rows_(rows), cols_(cols),
          values_(std::move(values)),
          column_indices_(std::move(column_indices)),
          row_pointers_(std::move(row_pointers)) {
        
        // Validate dimensions
        if (row_pointers_.size() != rows_ + 1) {
            throw std::invalid_argument("Row pointers size must be rows + 1");
        }
        
        if (values_.size() != column_indices_.size()) {
            throw std::invalid_argument("Values and column indices must have the same size");
        }
        
        if (row_pointers_.back() != values_.size()) {
            throw std::invalid_argument("Last row pointer must equal number of non-zero values");
        }
        
        // Validate column indices
        for (size_t i = 0; i < column_indices_.size(); ++i) {
            if (column_indices_[i] >= cols_) {
                throw std::invalid_argument("Column index out of bounds");
            }
        }
        
        // Validate row pointers (non-decreasing)
        for (size_t i = 1; i < row_pointers_.size(); ++i) {
            if (row_pointers_[i] < row_pointers_[i-1]) {
                throw std::invalid_argument("Row pointers must be non-decreasing");
            }
        }
    }

    CSRMatrix::CSRMatrix(const CSRMatrix& other)
        : rows_(other.rows_), cols_(other.cols_),
          values_(other.values_),
          column_indices_(other.column_indices_),
          row_pointers_(other.row_pointers_) {}

    CSRMatrix::CSRMatrix(CSRMatrix&& other) noexcept
        : rows_(other.rows_), cols_(other.cols_),
          values_(std::move(other.values_)),
          column_indices_(std::move(other.column_indices_)),
          row_pointers_(std::move(other.row_pointers_)) {}

    CSRMatrix& CSRMatrix::operator=(const CSRMatrix& other) {
        if (this != &other) {
            rows_ = other.rows_;
            cols_ = other.cols_;
            values_ = other.values_;
            column_indices_ = other.column_indices_;
            row_pointers_ = other.row_pointers_;
        }
        return *this;
    }

    CSRMatrix& CSRMatrix::operator=(CSRMatrix&& other) noexcept {
        if (this != &other) {
            rows_ = other.rows_;
            cols_ = other.cols_;
            values_ = std::move(other.values_);
            column_indices_ = std::move(other.column_indices_);
            row_pointers_ = std::move(other.row_pointers_);
        }
        return *this;
    }

    size_t CSRMatrix::rows() const {
        return rows_;
    }

    size_t CSRMatrix::cols() const {
        return cols_;
    }

    size_t CSRMatrix::nnz() const {
        return values_.size();
    }

    MatrixFormat CSRMatrix::format() const {
        return MatrixFormat::CSR;
    }

    SparseMatrix* CSRMatrix::to_format(MatrixFormat format) const {
        // TODO: Implement conversion to other formats
        // For now, just return a copy if requested format is CSR
        if (format == MatrixFormat::CSR) {
            return new CSRMatrix(*this);
        }
        
        // For other formats, return nullptr to indicate not implemented
        return nullptr;
    }

    void CSRMatrix::matvec(const double* x, double* y) const {
        // Perform matrix-vector multiplication
        for (size_t i = 0; i < rows_; ++i) {
            double sum = 0.0;
            for (size_t j = row_pointers_[i]; j < row_pointers_[i+1]; ++j) {
                sum += values_[j] * x[column_indices_[j]];
            }
            y[i] = sum;
        }
    }

    SparseMatrix* CSRMatrix::clone() const {
        return new CSRMatrix(*this);
    }

    const std::vector<double>& CSRMatrix::get_values() const {
        return values_;
    }

    const std::vector<size_t>& CSRMatrix::get_column_indices() const {
        return column_indices_;
    }

    const std::vector<size_t>& CSRMatrix::get_row_pointers() const {
        return row_pointers_;
    }

} // namespace SparseDirectMethod
