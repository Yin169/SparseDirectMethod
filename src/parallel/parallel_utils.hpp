#ifndef SPARSE_DIRECT_METHOD_PARALLEL_UTILS_HPP
#define SPARSE_DIRECT_METHOD_PARALLEL_UTILS_HPP

#include <cstddef>

namespace SparseDirectMethod {
namespace Parallel {

    /**
     * @brief Get the number of available threads
     * @return Number of available threads
     */
    size_t getNumThreads();

    /**
     * @brief Set the number of threads to use
     * @param numThreads Number of threads to use
     */
    void setNumThreads(size_t numThreads);

    /**
     * @brief Get the current thread ID
     * @return Current thread ID
     */
    size_t getThreadId();

    /**
     * @brief Parallel for loop implementation
     * @param start Start index
     * @param end End index
     * @param func Function to execute for each iteration
     */
    template<typename Func>
    void parallelFor(size_t start, size_t end, Func func);

    /**
     * @brief Parallel reduction implementation
     * @param start Start index
     * @param end End index
     * @param init Initial value
     * @param reduce Reduction operation
     * @param map Mapping operation
     * @return Reduced value
     */
    template<typename T, typename ReduceOp, typename MapOp>
    T parallelReduce(size_t start, size_t end, T init, ReduceOp reduce, MapOp map);

} // namespace Parallel
} // namespace SparseDirectMethod

#endif // SPARSE_DIRECT_METHOD_PARALLEL_UTILS_HPP
