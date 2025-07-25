#include "profiler.hpp"
#include <iomanip>

namespace SparseDirectMethod {
namespace Performance {

    Profiler& Profiler::getInstance() {
        static Profiler instance;
        return instance;
    }

    void Profiler::start(const std::string& name) {
        startTimes_[name] = std::chrono::high_resolution_clock::now();
    }

    void Profiler::stop(const std::string& name) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto startTime = startTimes_[name];
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        elapsedTimes_[name] = duration.count() / 1000000.0; // Convert to seconds
    }

    double Profiler::getElapsedTime(const std::string& name) const {
        auto it = elapsedTimes_.find(name);
        if (it != elapsedTimes_.end()) {
            return it->second;
        }
        return 0.0;
    }

    void Profiler::printResults() const {
        std::cout << "\n=== Performance Results ===" << std::endl;
        for (const auto& pair : elapsedTimes_) {
            std::cout << std::setw(20) << std::left << pair.first 
                      << ": " << std::fixed << std::setprecision(6) 
                      << pair.second << " seconds" << std::endl;
        }
        std::cout << "==========================\n" << std::endl;
    }

    void Profiler::reset() {
        startTimes_.clear();
        elapsedTimes_.clear();
    }

    ScopedTimer::ScopedTimer(const std::string& name) : name_(name) {
        Profiler::getInstance().start(name_);
    }

    ScopedTimer::~ScopedTimer() {
        Profiler::getInstance().stop(name_);
    }

} // namespace Performance
} // namespace SparseDirectMethod
