#ifndef SPARSE_DIRECT_METHOD_PROFILER_HPP
#define SPARSE_DIRECT_METHOD_PROFILER_HPP

#include <chrono>
#include <string>
#include <map>
#include <iostream>

namespace SparseDirectMethod {
namespace Performance {

    /**
     * @brief Simple performance profiler for timing code sections
     */
    class Profiler {
    public:
        /**
         * @brief Get the singleton instance of the profiler
         * @return Reference to the profiler instance
         */
        static Profiler& getInstance();

        /**
         * @brief Start timing a section of code
         * @param name Name of the section to profile
         */
        void start(const std::string& name);

        /**
         * @brief Stop timing a section of code
         * @param name Name of the section to profile
         */
        void stop(const std::string& name);

        /**
         * @brief Get the elapsed time for a section
         * @param name Name of the section
         * @return Elapsed time in seconds
         */
        double getElapsedTime(const std::string& name) const;

        /**
         * @brief Print all timing results
         */
        void printResults() const;

        /**
         * @brief Reset all timing data
         */
        void reset();

    private:
        /**
         * @brief Private constructor for singleton pattern
         */
        Profiler() = default;

        /**
         * @brief Map of start times for each section
         */
        std::map<std::string, std::chrono::high_resolution_clock::time_point> startTimes_;

        /**
         * @brief Map of elapsed times for each section
         */
        std::map<std::string, double> elapsedTimes_;
    };

    /**
     * @brief RAII helper class for automatically timing code sections
     */
    class ScopedTimer {
    public:
        /**
         * @brief Constructor that starts timing
         * @param name Name of the section to profile
         */
        explicit ScopedTimer(const std::string& name);

        /**
         * @brief Destructor that stops timing
         */
        ~ScopedTimer();

    private:
        std::string name_;
    };

} // namespace Performance
} // namespace SparseDirectMethod

#endif // SPARSE_DIRECT_METHOD_PROFILER_HPP
