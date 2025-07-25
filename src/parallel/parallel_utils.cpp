#include "parallel_utils.hpp"
#include <thread>
#include <vector>
#include <algorithm>
#include <mutex>
#include <numeric>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace SparseDirectMethod {
namespace Parallel {

    namespace {
        // Global variables for thread management
        size_t num_threads_ = std::thread::hardware_concurrency();
        thread_local size_t thread_id_ = 0;
    }

    size_t getNumThreads() {
#ifdef _OPENMP
        return omp_get_max_threads();
#else
        return num_threads_;
#endif
    }

    void setNumThreads(size_t numThreads) {
#ifdef _OPENMP
        omp_set_num_threads(static_cast<int>(numThreads));
#else
        num_threads_ = std::max(size_t(1), numThreads);
#endif
    }

    size_t getThreadId() {
#ifdef _OPENMP
        return static_cast<size_t>(omp_get_thread_num());
#else
        return thread_id_;
#endif
    }

    // Parallel for implementation
    template<typename Func>
    void parallelFor(size_t start, size_t end, Func func) {
        if (start >= end) return;
        
#ifdef _OPENMP
        #pragma omp parallel for
        for (size_t i = start; i < end; ++i) {
            func(i);
        }
#else
        // Sequential fallback
        for (size_t i = start; i < end; ++i) {
            func(i);
        }
#endif
    }

    // Parallel reduction implementation
    template<typename T, typename ReduceOp, typename MapOp>
    T parallelReduce(size_t start, size_t end, T init, ReduceOp reduce, MapOp map) {
        if (start >= end) return init;
        
#ifdef _OPENMP
        T result = init;
        #pragma omp parallel
        {
            T local_result = init;
            #pragma omp for
            for (size_t i = start; i < end; ++i) {
                local_result = reduce(local_result, map(i));
            }
            #pragma omp critical
            {
                result = reduce(result, local_result);
            }
        }
        return result;
#else
        // Sequential fallback
        T result = init;
        for (size_t i = start; i < end; ++i) {
            result = reduce(result, map(i));
        }
        return result;
#endif
    }

    // Explicit template instantiations for common types
    template void parallelFor<std::function<void(size_t)>>(size_t, size_t, std::function<void(size_t)>);
    
    template double parallelReduce<double, std::function<double(double, double)>, std::function<double(size_t)>>(
        size_t, size_t, double, std::function<double(double, double)>, std::function<double(size_t)>);
    
    template int parallelReduce<int, std::function<int(int, int)>, std::function<int(size_t)>>(
        size_t, size_t, int, std::function<int(int, int)>, std::function<int(size_t)>);

} // namespace Parallel
} // namespace SparseDirectMethod
