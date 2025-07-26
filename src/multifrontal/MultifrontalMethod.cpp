#include "MultifrontalMethod.h"
#include <cstring>
#include <iostream>

MultifrontalMethod::MultifrontalMethod() : 
    m_n(0), m_ap(nullptr), m_ai(nullptr), m_ax(nullptr), 
    m_symbolic(nullptr), m_numeric(nullptr), m_factorized(false) {
    // Constructor
}

MultifrontalMethod::~MultifrontalMethod() {
    // Free UMFPACK structures
    if (m_symbolic) {
        umfpack_di_free_symbolic(&m_symbolic);
    }
    
    if (m_numeric) {
        umfpack_di_free_numeric(&m_numeric);
    }
    
    // Note: We don't own m_ap, m_ai, m_ax, so we don't free them
}

bool MultifrontalMethod::setMatrix(int n, const int* ap, const int* ai, const double* ax) {
    if (n <= 0 || !ap || !ai || !ax) {
        return false;
    }
    
    // Store matrix dimensions
    m_n = n;
    
    // Free previous symbolic and numeric factorization
    if (m_symbolic) {
        umfpack_di_free_symbolic(&m_symbolic);
        m_symbolic = nullptr;
    }
    
    if (m_numeric) {
        umfpack_di_free_numeric(&m_numeric);
        m_numeric = nullptr;
    }
    
    m_factorized = false;
    
    // Store matrix data (shallow copy - we assume these arrays will remain valid)
    m_ap = const_cast<int*>(ap);
    m_ai = const_cast<int*>(ai);
    m_ax = const_cast<double*>(ax);
    
    return true;
}

bool MultifrontalMethod::factorize() {
    if (m_n <= 0 || !m_ap || !m_ai || !m_ax) {
        std::cerr << "Matrix not set" << std::endl;
        return false;
    }
    
    // Free previous factorizations
    if (m_symbolic) {
        umfpack_di_free_symbolic(&m_symbolic);
        m_symbolic = nullptr;
    }
    
    if (m_numeric) {
        umfpack_di_free_numeric(&m_numeric);
        m_numeric = nullptr;
    }
    
    // Symbolic factorization
    int status = umfpack_di_symbolic(m_n, m_n, m_ap, m_ai, m_ax, &m_symbolic, nullptr, nullptr);
    if (status != UMFPACK_OK) {
        std::cerr << "UMFPACK symbolic factorization failed with status: " << status << std::endl;
        return false;
    }
    
    // Numeric factorization
    status = umfpack_di_numeric(m_ap, m_ai, m_ax, m_symbolic, &m_numeric, nullptr, nullptr);
    if (status != UMFPACK_OK) {
        std::cerr << "UMFPACK numeric factorization failed with status: " << status << std::endl;
        return false;
    }
    
    m_factorized = true;
    return true;
}

bool MultifrontalMethod::solve(const double* b, double* x) {
    if (!m_factorized || !b || !x) {
        return false;
    }
    
    // Solve the system Ax = b
    int status = umfpack_di_solve(UMFPACK_A, m_ap, m_ai, m_ax, x, b, m_numeric, nullptr, nullptr);
    if (status != UMFPACK_OK) {
        std::cerr << "UMFPACK solve failed with status: " << status << std::endl;
        return false;
    }
    
    return true;
}

void MultifrontalMethod::getInfo(double info[91]) const {
    // Clear the info array
    for (int i = 0; i < 91; i++) {
        info[i] = 0.0;
    }
    
    // Note: The umfpack_di_get_numeric function signature has changed in newer versions
    // For now, we'll just clear the info array. In a full implementation, we would
    // extract relevant information from the numeric factorization.
}