#ifndef SPARSE_DIRECT_METHOD_H
#define SPARSE_DIRECT_METHOD_H

#include <cholmod.h>
#include <umfpack.h>
#include <cblas.h>

/**
 * @brief Base class for sparse direct methods
 */
class SparseDirectMethod {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~SparseDirectMethod() = default;
    
    /**
     * @brief Factorize the sparse matrix
     * @return true if successful, false otherwise
     */
    virtual bool factorize() = 0;
    
    /**
     * @brief Solve the linear system Ax = b
     * @param b Right-hand side vector
     * @param x Solution vector
     * @return true if successful, false otherwise
     */
    virtual bool solve(const double* b, double* x) = 0;
};

#endif // SPARSE_DIRECT_METHOD_H