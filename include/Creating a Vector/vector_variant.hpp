#pragma once

#include "vector.hpp"
#include <algorithm>

// A vector class that uses the STL cppy function
template <typename T>
class VectorSTLCopy : public Vector<T> {
protected:
    // Copies elements from the source array to the destination array
    void copyElements(T* dest, const T* src, size_t n) const override {
        std::copy(src, src + n, dest);
    }
};

// A Vector class that increases its capacity by a larger amount
template <typename T>
class VectorBigStep : public Vector<T> {
protected:
    // Returns the next capacity for the vector
    size_t nextCapacity(size_t currentCapacity) const override {
        // Start with 128 if the current capacity is 0
        // Otherwise, increase the capacity by 4 times

        return (currentCapacity == 0) ? 128 : currentCapacity * 4;
    }
};