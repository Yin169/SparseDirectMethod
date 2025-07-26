#ifndef PARALLEL_MULTIFRONTAL_METHOD_H
#define PARALLEL_MULTIFRONTAL_METHOD_H

#include "../core/SparseDirectMethod.h"
#include <umfpack.h>
#include <omp.h>

/**
 * @brief Parallel Multifrontal method implementation using UMFPACK from SuiteSparse
 * 
 * This implementation extends the basic multifrontal approach with parallel processing
 * capabilities to handle large-scale matrices more efficiently. It uses OpenMP for 
 * shared-memory parallelism and can take advantage of multiple CPU cores.
 * 
 * The parallelization strategies include:
 * 1. Parallel assembly of frontal matrices
 * 2. Parallel processing of independent fronts
 * 3. Parallel BLAS operations within each front
 */
class ParallelMultifrontalMethod : public SparseDirectMethod {
private:
    // Matrix in compressed column form
    int m_n;                      // Matrix order
    int* m_ap;                    // Column pointers (size n+1)
    int* m_ai;                    // Row indices (size nz)
    double* m_ax;                 // Numerical values (size nz)
    mutable void* m_symbolic;     // Symbolic factorization
    mutable void* m_numeric;      // Numeric factorization
    bool m_factorized;
    
    // Parallel processing parameters
    int m_num_threads;            // Number of threads to use

public:
    /**
     * @brief Constructor
     * @param num_threads Number of threads to use (default: 0 means use system default)
     */
    ParallelMultifrontalMethod(int num_threads = 0);
    
    /**
     * @brief Destructor
     */
    ~ParallelMultifrontalMethod();
    
    /**
     * @brief Set the matrix to be factorized in CSC format
     * @param n Matrix order
     * @param ap Column pointers array
     * @param ai Row indices array
     * @param ax Numerical values array
     * @return true if successful, false otherwise
     */
    bool setMatrix(int n, const int* ap, const int* ai, const double* ax);
    
    /**
     * @brief Factorize the sparse matrix using parallel multifrontal approach
     * @return true if successful, false otherwise
     */
    bool factorize() override;
    
    /**
     * @brief Solve the linear system Ax = b in parallel
     * @param b Right-hand side vector
     * @param x Solution vector
     * @return true if successful, false otherwise
     */
    bool solve(const double* b, double* x) override;
    
    /**
     * @brief Get information about the factorization
     * @param info Array to store information (size 91)
     */
    void getInfo(double info[91]) const;
    
    /**
     * @brief Set the number of threads to use
     * @param num_threads Number of threads
     */
    void setNumThreads(int num_threads);
    
    /**
     * @brief Get the number of threads being used
     * @return Number of threads
     */
    int getNumThreads() const;
};

#endif // PARALLEL_MULTIFRONTAL_METHOD_H