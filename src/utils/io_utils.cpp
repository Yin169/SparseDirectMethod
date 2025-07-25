#include "io_utils.hpp"
#include "core/matrix/csr_matrix.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <numeric>

namespace SparseDirectMethod {
namespace Utils {

    std::unique_ptr<SparseMatrix> read_matrix_market(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        
        std::string line;
        std::string header;
        std::string format;
        std::string qualifier;
        std::string symmetry;
        
        // Read header line
        std::getline(file, line);
        std::istringstream header_stream(line);
        header_stream >> header >> format >> qualifier >> symmetry;
        
        // Check if it's a matrix market file
        if (header != "%%MatrixMarket") {
            throw std::runtime_error("Not a valid Matrix Market file");
        }
        
        // Check if it's a matrix (not an array)
        if (format != "matrix") {
            throw std::runtime_error("File does not contain a matrix");
        }
        
        // Skip comments
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '%') {
                continue;
            }
            break;
        }
        
        // Parse matrix dimensions
        std::istringstream dim_stream(line);
        size_t rows, cols, nnz;
        dim_stream >> rows >> cols >> nnz;
        
        // Read matrix data
        std::vector<double> values;
        std::vector<size_t> row_indices;
        std::vector<size_t> col_indices;
        
        values.reserve(nnz);
        row_indices.reserve(nnz);
        col_indices.reserve(nnz);
        
        for (size_t i = 0; i < nnz; ++i) {
            if (!std::getline(file, line)) {
                throw std::runtime_error("Unexpected end of file");
            }
            
            std::istringstream data_stream(line);
            size_t row, col;
            double value;
            
            if (qualifier == "pattern") {
                data_stream >> row >> col;
                value = 1.0;
            } else {
                data_stream >> row >> col >> value;
            }
            
            // Matrix Market uses 1-based indexing
            row_indices.push_back(row - 1);
            col_indices.push_back(col - 1);
            values.push_back(value);
        }
        
        file.close();
        
        // Convert to CSR format
        // Sort by row and then by column
        std::vector<size_t> indices(nnz);
        std::iota(indices.begin(), indices.end(), 0);
        
        std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
            if (row_indices[a] != row_indices[b]) {
                return row_indices[a] < row_indices[b];
            }
            return col_indices[a] < col_indices[b];
        });
        
        std::vector<double> sorted_values(nnz);
        std::vector<size_t> sorted_col_indices(nnz);
        std::vector<size_t> row_pointers(rows + 1, 0);
        
        for (size_t i = 0; i < nnz; ++i) {
            sorted_values[i] = values[indices[i]];
            sorted_col_indices[i] = col_indices[indices[i]];
        }
        
        // Build row pointers
        size_t current_row = 0;
        for (size_t i = 0; i < nnz; ++i) {
            while (row_indices[indices[i]] > current_row) {
                row_pointers[++current_row] = i;
            }
        }
        while (current_row < rows) {
            row_pointers[++current_row] = nnz;
        }
        
        return std::make_unique<CSRMatrix>(rows, cols,
                                         std::move(sorted_values),
                                         std::move(sorted_col_indices),
                                         std::move(row_pointers));
    }

    void write_matrix_market(const SparseMatrix& matrix, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        // Write header
        file << "%%MatrixMarket matrix coordinate real general\n";
        
        // Write dimensions
        file << matrix.rows() << " " << matrix.cols() << " " << matrix.nnz() << "\n";
        
        // For now, we only support CSR matrices for writing
        if (matrix.format() == MatrixFormat::CSR) {
            const CSRMatrix* csr_matrix = static_cast<const CSRMatrix*>(&matrix);
            const std::vector<double>& values = csr_matrix->get_values();
            const std::vector<size_t>& column_indices = csr_matrix->get_column_indices();
            const std::vector<size_t>& row_pointers = csr_matrix->get_row_pointers();
            
            // Write matrix data (1-based indexing)
            for (size_t i = 0; i < matrix.rows(); ++i) {
                for (size_t j = row_pointers[i]; j < row_pointers[i + 1]; ++j) {
                    file << (i + 1) << " " << (column_indices[j] + 1) << " " 
                         << std::setprecision(15) << values[j] << "\n";
                }
            }
        } else {
            throw std::runtime_error("Writing non-CSR matrices not supported yet");
        }
        
        file.close();
    }

    double* read_dense_vector(const std::string& filename, size_t& size) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        
        // Read all values into a vector first to determine size
        std::vector<double> values;
        double value;
        while (file >> value) {
            values.push_back(value);
        }
        
        file.close();
        
        // Allocate and copy to array
        size = values.size();
        double* result = new double[size];
        std::copy(values.begin(), values.end(), result);
        
        return result;
    }

    void write_dense_vector(const double* vector, size_t size, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        // Write vector values
        for (size_t i = 0; i < size; ++i) {
            file << std::setprecision(15) << vector[i] << "\n";
        }
        
        file.close();
    }

} // namespace Utils
} // namespace SparseDirectMethod
