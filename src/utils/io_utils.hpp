#ifndef SPARSE_DIRECT_METHOD_IO_UTILS_HPP
#define SPARSE_DIRECT_METHOD_IO_UTILS_HPP

#include "core/matrix/sparse_matrix.hpp"
#include <string>
#include <memory>

namespace SparseDirectMethod {
namespace Utils {

    /**
     * @brief Read a sparse matrix from a Matrix Market file
     * @param filename Path to the Matrix Market file
     * @return Unique pointer to the sparse matrix
     */
    std::unique_ptr<SparseMatrix> read_matrix_market(const std::string& filename);

    /**
     * @brief Write a sparse matrix to a Matrix Market file
     * @param matrix The sparse matrix to write
     * @param filename Path to the output file
     */
    void write_matrix_market(const SparseMatrix& matrix, const std::string& filename);

    /**
     * @brief Read a dense vector from a text file
     * @param filename Path to the text file
     * @param size Output parameter for the vector size
     * @return Pointer to the vector data (caller is responsible for deleting)
     */
    double* read_dense_vector(const std::string& filename, size_t& size);

    /**
     * @brief Write a dense vector to a text file
     * @param vector The vector data
     * @param size The size of the vector
     * @param filename Path to the output file
     */
    void write_dense_vector(const double* vector, size_t size, const std::string& filename);

} // namespace Utils
} // namespace SparseDirectMethod

#endif // SPARSE_DIRECT_METHOD_IO_UTILS_HPP
