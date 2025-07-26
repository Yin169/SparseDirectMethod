#include "dense_matrix.hpp"
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <cstring>

namespace SparseDirectMethod {
namespace Matrix {

    DenseMatrix::DenseMatrix(size_t rows, size_t cols)
        : rows_(rows), cols_(cols), data_(rows * cols, 0.0) {
        if (rows == 0 || cols == 0) {
            throw std::invalid_argument("Matrix dimensions must be positive");
        }
    }

    DenseMatrix::DenseMatrix(const DenseMatrix& other)
        : rows_(other.rows_), cols_(other.cols_), data_(other.data_) {}

    DenseMatrix::DenseMatrix(DenseMatrix&& other) noexcept
        : rows_(other.rows_), cols_(other.cols_), data_(std::move(other.data_)) {}

    DenseMatrix& DenseMatrix::operator=(const DenseMatrix& other) {
        if (this != &other) {
            rows_ = other.rows_;
            cols_ = other.cols_;
            data_ = other.data_;
        }
        return *this;
    }

    DenseMatrix& DenseMatrix::operator=(DenseMatrix&& other) noexcept {
        if (this != &other) {
            rows_ = other.rows_;
            cols_ = other.cols_;
            data_ = std::move(other.data_);
        }
        return *this;
    }

    size_t DenseMatrix::rows() const {
        return rows_;
    }

    size_t DenseMatrix::cols() const {
        return cols_;
    }

    double& DenseMatrix::operator()(size_t i, size_t j) {
        if (i >= rows_ || j >= cols_) {
            throw std::out_of_range("Matrix indices out of range");
        }
        return data_[i * cols_ + j];
    }

    const double& DenseMatrix::operator()(size_t i, size_t j) const {
        if (i >= rows_ || j >= cols_) {
            throw std::out_of_range("Matrix indices out of range");
        }
        return data_[i * cols_ + j];
    }

    double* DenseMatrix::data() {
        return data_.data();
    }

    const double* DenseMatrix::data() const {
        return data_.data();
    }

    void DenseMatrix::zero() {
        std::fill(data_.begin(), data_.end(), 0.0);
    }

    void DenseMatrix::lu_factorize(std::vector<int>& permutations) {
        if (rows_ != cols_) {
            throw std::invalid_argument("LU factorization requires a square matrix");
        }

        size_t n = rows_;
        permutations.resize(n);
        for (size_t i = 0; i < n; ++i) {
            permutations[i] = static_cast<int>(i);
        }

        for (size_t k = 0; k < n; ++k) {
            // Find pivot
            size_t pivot_row = k;
            double max_val = std::abs((*this)(k, k));

            for (size_t i = k + 1; i < n; ++i) {
                double val = std::abs((*this)(i, k));
                if (val > max_val) {
                    max_val = val;
                    pivot_row = i;
                }
            }

            // Swap rows if needed
            if (pivot_row != k) {
                for (size_t j = 0; j < n; ++j) {
                    std::swap((*this)(k, j), (*this)(pivot_row, j));
                }
                std::swap(permutations[k], permutations[pivot_row]);
            }

            // Check for zero pivot
            if (std::abs((*this)(k, k)) < 1e-12) {
                throw std::runtime_error("Matrix is singular or nearly singular");
            }

            // Compute multipliers and update matrix
            for (size_t i = k + 1; i < n; ++i) {
                (*this)(i, k) /= (*this)(k, k);
                for (size_t j = k + 1; j < n; ++j) {
                    (*this)(i, j) -= (*this)(i, k) * (*this)(k, j);
                }
            }
        }
    }

    void DenseMatrix::lu_solve(const std::vector<int>& permutations, const double* b, double* x) const {
        if (rows_ != cols_) {
            throw std::invalid_argument("LU solve requires a square matrix");
        }

        size_t n = rows_;

        // Apply row permutations to b
        std::vector<double> pb(n);
        for (size_t i = 0; i < n; ++i) {
            pb[i] = b[permutations[i]];
        }

        // Forward substitution: Ly = Pb
        std::vector<double> y(n, 0.0);
        for (size_t i = 0; i < n; ++i) {
            y[i] = pb[i];
            for (size_t j = 0; j < i; ++j) {
                y[i] -= (*this)(i, j) * y[j];
            }
        }

        // Backward substitution: Ux = y
        for (size_t i = n; i > 0; --i) {
            size_t idx = i - 1;
            x[idx] = y[idx];
            for (size_t j = idx + 1; j < n; ++j) {
                x[idx] -= (*this)(idx, j) * x[j];
            }
            x[idx] /= (*this)(idx, idx);
        }
    }

    void DenseMatrix::matvec(const double* x, double* y) const {
        for (size_t i = 0; i < rows_; ++i) {
            y[i] = 0.0;
            for (size_t j = 0; j < cols_; ++j) {
                y[i] += (*this)(i, j) * x[j];
            }
        }
    }

    void DenseMatrix::add_contribution(const DenseMatrix& other, size_t row_offset, size_t col_offset) {
        if (row_offset + other.rows() > rows_ || col_offset + other.cols() > cols_) {
            throw std::out_of_range("Contribution matrix exceeds bounds");
        }

        for (size_t i = 0; i < other.rows(); ++i) {
            for (size_t j = 0; j < other.cols(); ++j) {
                (*this)(row_offset + i, col_offset + j) += other(i, j);
            }
        }
    }

    DenseMatrix DenseMatrix::extract_submatrix(size_t row_start, size_t row_end, 
                                              size_t col_start, size_t col_end) const {
        if (row_start >= row_end || col_start >= col_end ||
            row_end > rows_ || col_end > cols_) {
            throw std::out_of_range("Invalid submatrix indices");
        }

        size_t sub_rows = row_end - row_start;
        size_t sub_cols = col_end - col_start;

        DenseMatrix result(sub_rows, sub_cols);
        for (size_t i = 0; i < sub_rows; ++i) {
            for (size_t j = 0; j < sub_cols; ++j) {
                result(i, j) = (*this)(row_start + i, col_start + j);
            }
        }

        return result;
    }

    std::unique_ptr<DenseMatrix> DenseMatrix::clone() const {
        return std::make_unique<DenseMatrix>(*this);
    }

} // namespace Matrix
} // namespace SparseDirectMethod
