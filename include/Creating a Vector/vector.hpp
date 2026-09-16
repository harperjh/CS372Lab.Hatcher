#pragma once

#include <cstddef>
#include <stdexcept>

// A simple vector class that stores elements in a dynamic array
template <typename T>
class Vector {
protected:
    T* data;   // Stores the elements
    size_t count;  // Number of elements currently stored
    size_t capacity; // Total space available
    
    // Calculates the next capacity when the vector needs more space
    virtual size_t nextCapacity(size_t currentCapacity) const {
        return (currentCapacity == 0) ? 1 : currentCapacity * 2;
    }
    
    // Copies elements from one array to another
    virtual void copyElements(T* dest, const T* src, size_t n) const {
        for (size_t i = 0; i < n; ++i) {
            dest[i] = src[i];
        }
    }

    // Makes the array bigger when it is full
    void grow() {
        size_t newCapacity = nextCapacity(capacity);
        T* newData = new T[newCapacity];
        copyElements(newData, data, count);
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    // Default constructor
    Vector() : data(nullptr), count(0), capacity(0) {}
    // Copy constructor
    Vector(const Vector& other)
        : data(nullptr), count(0), capacity(0) {
        if (other.capacity > 0) {
            data = new T[other.capacity];
            capacity = other.capacity;
        }
        // Copy all elements from the other vector
        for (size_t i = 0; i < other.count; ++i) {
            data[i] = other.data[i];
        }
        count = other.count;
    }
    // Assignment operator
    Vector& operator=(const Vector& other) {
        if (this == &other) return *this;
        delete[] data;
        data = nullptr;
        capacity = other.capacity;
        count = other.count;
        // Create new space and copy the elements
        if (capacity > 0) {
            data = new T[capacity];
            for (size_t i = 0; i < count; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }
    // Destructor
    virtual ~Vector() {
        delete[] data;
    }
    // Adds a new element to the end-of the vector
    void push_back(const T& value) {
        // Grow the array if it is full
        if (count == capacity) {
            grow();
        }
        data[count++] = value;
    }
    // Returns the number of element
    size_t size() const { return count; }
    // Returns the total capacity
    size_t getCapacity() const { return capacity; }
    // Checks if the vector is empty
    bool empty() const { return count == 0; }

    // Accesses an element by index
    T& operator[](size_t index) {
        return data[index];
    }
    // Accesses an element by index for const vectors
    const T& operator[](size_t index) const {
        return data[index];
    }
    
    // Accesses an element and checks if the index is valid
    T& at(size_t index) {
        if (index >= count) throw std::out_of_range("Vector::at index out of range");
        return data[index];
    }
};