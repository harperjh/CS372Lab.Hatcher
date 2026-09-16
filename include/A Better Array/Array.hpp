#pragma once

#include <cstddef>
#include <stdexcept>

template <typename T, size_t N>
class Array {
private:
    T data[N];

public:
    explicit Array(size_t size = N) {
        if (size != N) {
            throw std::invalid_argument("Array: requested size does not match template size N");
        }
    }

    Array(const Array& other) {
        for (size_t i = 0; i < N; ++i) {
            data[i] = other.data[i];
        }
    }

    Array& operator=(const Array& other) {
        if (this == &other) return *this;
        for (size_t i = 0; i < N; ++i) {
            data[i] = other.data[i];
        }
        return *this;
    }

    size_t size() const { return N; }

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

    T& at(size_t index) {
        if (index >= N) throw std::out_of_range("Array::at index out of range");
        return data[index];
    }
};