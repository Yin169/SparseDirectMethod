#ifndef SUPER_NODE_METHOD_H
#define SUPER_NODE_METHOD_H

#include "../core/SparseDirectMethod.h"
#include <cholmod.h>

/**
 * @brief Supernodal method implementation using CHOLMOD from SuiteSparse
 * 
 * The supernodal method is an efficient approach for sparse symmetric positive definite
 * matrix factorization. It groups columns with similar structures into supernodes
 * and performs dense matrix operations on these supernodes for better performance.
 */
class SupernodalMethod : public SparseDirectMethod {
private:
    cholmod_common m_common;
    cholmod_sparse* m_A;
    cholmod_factor* m_L;
    bool m_factorized;

public:
    /**
     * @brief Constructor
     */
    SupernodalMethod();
    
    /**
     * @brief Destructor
     */
    ~SupernodalMethod();
    
    /**
     * @brief Set the matrix to be factorized
     * @param A The sparse matrix in CHOLMOD format
     * @return true if successful, false otherwise
     */
    bool setMatrix(cholmod_sparse* A);
    
    /**
     * @brief Factorize the sparse matrix using supernodal approach
     * @return true if successful, false otherwise
     */
    bool factorize() override;
    
    /**
     * @brief Solve the linear system Ax = b
     * @param b Right-hand side vector
     * @param x Solution vector
     * @return true if successful, false otherwise
     */
    bool solve(const double* b, double* x) override;
    
    /**
     * @brief Get the Cholesky factor L
     * @return Pointer to the Cholesky factor
     */
    cholmod_factor* getFactor() const;
};

#endif // SUPER_NODE_METHOD_H