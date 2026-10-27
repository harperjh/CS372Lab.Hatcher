//
// File: pair.hpp
// Purpose:
// A simple key/value pair. The defaulted spaceship operator makes the
// compiler generate ==, !=, <, <=, >, >= (compares `first`, then `second`).
//
#pragma once

#include <compare>
#include <iostream>

template <typename Key, typename Value>
struct Pair {
    Key first;
    Value second;

    auto operator<=>(const Pair &) const = default;
};

template <typename Key, typename Value>
std::ostream &operator<<(std::ostream &os, const Pair<Key, Value> &p) {
    return os << '(' << p.first << ", " << p.second << ')';
}
