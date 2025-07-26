#include "SupernodalMethod.h"
#include <iostream>

SupernodalMethod::SupernodalMethod() : m_A(nullptr), m_L(nullptr), m_factorized(false) {
    // Initialize CHOLMOD
    cholmod_start(&m_common);
    
    // Set supernodal memory management parameters
    m_common.supernodal = CHOLMOD_SUPERNODAL;
    
    // Enable OpenBLAS for dense operations (if available)
    // Note: This is a common parameter, but actual BLAS library linking is done at compile time
    m_common.nmethods = 1;
    m_common.method[0].ordering = CHOLMOD_AMD;
}

SupernodalMethod::~SupernodalMethod() {
    // Free CHOLMOD structures
    if (m_L) {
        cholmod_free_factor(&m_L, &m_common);
    }
    
    // Finish CHOLMOD
    cholmod_finish(&m_common);
}

bool SupernodalMethod::setMatrix(cholmod_sparse* A) {
    if (!A) {
        return false;
    }
    
    // Free previous matrix if exists
    if (m_A) {
        // Note: m_A is typically not owned by this class, so we don't free it
        m_A = nullptr;
    }
    
    // Reset factorization status
    m_factorized = false;
    if (m_L) {
        cholmod_free_factor(&m_L, &m_common);
        m_L = nullptr;
    }
    
    m_A = A;
    return true;
}

bool SupernodalMethod::factorize() {
    if (!m_A) {
        std::cerr << "Matrix not set" << std::endl;
        return false;
    }
    
    // Perform Cholesky factorization using supernodal method
    m_L = cholmod_analyze(m_A, &m_common);
    if (!m_L) {
        std::cerr << "CHOLMOD analysis failed" << std::endl;
        return false;
    }
    
    int status = cholmod_factorize(m_A, m_L, &m_common);
    if (!status) {
        std::cerr << "CHOLMOD factorization failed" << std::endl;
        return false;
    }
    
    m_factorized = true;
    return true;
}

bool SupernodalMethod::solve(const double* b, double* x) {
    if (!m_factorized || !b || !x) {
        return false;
    }
    
    // Create dense vector from b
    cholmod_dense* B = cholmod_zeros(m_A->nrow, 1, CHOLMOD_REAL, &m_common);
    if (!B) {
        return false;
    }
    
    // Copy b to B
    for (size_t i = 0; i < m_A->nrow; i++) {
        ((double*)B->x)[i] = b[i];
    }
    
    // Solve the system
    cholmod_dense* X = cholmod_solve(CHOLMOD_A, m_L, B, &m_common);
    if (!X) {
        cholmod_free_dense(&B, &m_common);
        return false;
    }
    
    // Copy result to x
    for (size_t i = 0; i < m_A->nrow; i++) {
        x[i] = ((double*)X->x)[i];
    }
    
    // Free temporary vectors
    cholmod_free_dense(&B, &m_common);
    cholmod_free_dense(&X, &m_common);
    
    return true;
}

cholmod_factor* SupernodalMethod::getFactor() const {
    return m_L;
}