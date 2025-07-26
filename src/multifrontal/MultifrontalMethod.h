#ifndef MULTIFRONTAL_METHOD_H
#define MULTIFRONTAL_METHOD_H

#include "../core/SparseDirectMethod.h"
#include <umfpack.h>

/**
 * @brief Multifrontal method implementation using UMFPACK from SuiteSparse
 * 
 * The multifrontal method is a direct method for solving sparse linear systems.
 * It works with the elimination tree and processes multiple fronts simultaneously.
 * This implementation uses UMFPACK which is a multifrontal method for unsymmetric
 * sparse matrices.
 */
class MultifrontalMethod : public SparseDirectMethod {
private:
    // Matrix in compressed column form
    int m_n;                      // Matrix order
    int* m_ap;                    // Column pointers (size n+1)
    int* m_ai;                    // Row indices (size nz)
    double* m_ax;                 // Numerical values (size nz)
    mutable void* m_symbolic;     // Symbolic factorization
    mutable void* m_numeric;      // Numeric factorization
    bool m_factorized;

public:
    /**
     * @brief Constructor
     */
    MultifrontalMethod();
    
    /**
     * @brief Destructor
     */
    ~MultifrontalMethod();
    
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
     * @brief Factorize the sparse matrix using multifrontal approach
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
     * @brief Get information about the factorization
     * @param info Array to store information (size 91)
     */
    void getInfo(double info[91]) const;
};

#endif // MULTIFRONTAL_METHOD_H