#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include "vector.hpp"
#include "vector_variant.hpp"

// Measures how long it takes to add n elements to the vector
template <typename VecType>
double timePushBacks(size_t n) {
    VecType v;
    // Start timer
    auto start = std::chrono::high_resolution_clock::now();
    // Add elements to the vector
    for (size_t i = 0; i < n; ++i) {
        v.push_back(static_cast<int>(i));
    }
    // Stop the timer
    auto end = std::chrono::high_resolution_clock::now();
    // Calcuate the elapsed time in milliseconds
    std::chrono::duration<double, std::milli> elapsed = end - start;
    return elapsed.count();
}

// Runs a test for different vector sizes
template <typename VecType>
void runTrial(const std::string& label, const std::vector<size_t>& sizes) {
    std::cout << "\n=== " << label << " ===\n";
    // Print the table headings
    std::cout << std::left << std::setw(12) << "n"
        << std::setw(15) << "time (ms)" << "\n";
    // Test each size and print the time
    for (size_t n : sizes) {
        double ms = timePushBacks<VecType>(n);
        std::cout << std::left << std::setw(12) << n
            << std::setw(15) << ms << "\n";
    }
}

int main() {
    std::vector<size_t> sizes;
    // Create test sizes feom 2 to 4096
    for (size_t n = 2; n <= 4096; n *= 2) {
        sizes.push_back(n);
    }
    // Test the base vector
    runTrial<Vector<int>>("Base Vector (double from 1, for-loop copy)", sizes);
    // Test the vector that uses std:: copy
    runTrial<VectorSTLCopy<int>>("VectorSTLCopy (double from 1, std::copy)", sizes);
    // Test the vector with larger capacity increases
    runTrial<VectorBigStep<int>>("VectorBigStep (start 128, x4 growth, for-loop copy)", sizes);

    return 0;
}