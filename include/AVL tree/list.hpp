//
// File: list.hpp
// Purpose:
// STAND-IN ONLY so dictionary.hpp compiles and can be tested here.
// Replace with your class library's List<T>. The Dictionary only needs
// a default constructor, move assignment, and push_back(const T&).
//
#pragma once

#include <cstddef>
#include <vector>

template <typename T>
class List {
    std::vector<T> data_;
public:
    void push_back(const T &v) { data_.push_back(v); }
    std::size_t size() const noexcept { return data_.size(); }
    bool isEmpty() const noexcept { return data_.empty(); }
    const T &operator[](std::size_t i) const { return data_[i]; }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
};
