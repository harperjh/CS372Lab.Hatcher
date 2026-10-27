//
// File: main.cpp
// Purpose: Test driver for Pair and Dictionary.
//
#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <string>

#include "dictionary.hpp"
#include "pair.hpp"

static void testPair() {
    Pair<int, std::string> a{1, "one"}, b{2, "two"}, c{1, "uno"};
    assert(a < b && b > a && a <= b && a != b);
    assert(a < c);                                  // same key: falls back to `second`
    assert((a == Pair<int, std::string>{1, "one"}));
    std::cout << "Pair: " << a << " < " << b << "  ok\n";
}

static void testBasics() {
    Dictionary<std::string, int> d;
    assert(d.isEmpty() && d.size() == 0);

    assert(d.insert("banana", 3));
    assert(d.insert({"apple", 5}));
    assert(!d.insert("apple", 99));                 // duplicate key rejected
    assert(d.at("apple") == 5);

    d["cherry"] = 7;                                // operator[] inserts
    ++d["apple"];                                   // operator[] modifies in place
    assert(d["grape"] == 0 && d.size() == 4);       // default-inserted

    bool threw = false;
    try { d.at("kiwi"); } catch (const std::out_of_range &) { threw = true; }
    assert(threw);

    auto &ks = d.keys();
    auto &vs = d.values();
    std::cout << "keys:   "; for (auto &k : ks) std::cout << k << ' '; std::cout << '\n';
    std::cout << "values: "; for (auto &v : vs) std::cout << v << ' '; std::cout << '\n';
    assert(ks.size() == 4 && ks[0] == "apple" && ks[3] == "grape");
    assert(vs[0] == 6 && vs[1] == 3 && vs[2] == 7 && vs[3] == 0);

    assert(d.erase("banana") && !d.erase("banana") && d.size() == 3);

    // Rule of Five
    Dictionary<std::string, int> copy = d;          // copy ctor
    copy["apple"] = 100;
    assert(d.at("apple") == 6);                     // deep copy
    Dictionary<std::string, int> assigned;
    assigned = copy;                                // copy assignment
    assert(assigned.at("apple") == 100);
    Dictionary<std::string, int> moved = std::move(assigned);   // move ctor
    assert(moved.size() == 3 && assigned.isEmpty());
    Dictionary<std::string, int> moveAssigned;
    moveAssigned = std::move(moved);                // move assignment
    assert(moveAssigned.size() == 3 && moved.isEmpty());

    d.clear();
    assert(d.isEmpty() && d.keys().size() == 0);
    d["after"] = 1;                                 // usable after clear
    assert(d.size() == 1);

    std::cout << "Dictionary basics ok\n";
}

static void testRandomAgainstStdMap() {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> key(0, 500), op(0, 3);
    Dictionary<int, int> d;
    std::map<int, int> ref;

    for (int i = 0; i < 200000; ++i) {
        int k = key(rng);
        switch (op(rng)) {
        case 0: assert(d.insert(k, i) == ref.insert({k, i}).second); break;
        case 1: d[k] += i; ref[k] += i; break;
        case 2: assert(d.erase(k) == (ref.erase(k) == 1)); break;
        case 3:
            if (ref.count(k)) assert(d.at(k) == ref.at(k));
            else assert(!d.contains(k));
            break;
        }
        assert(d.size() == ref.size());
    }
    auto &ks = d.keys();
    auto &vs = d.values();
    std::size_t i = 0;
    for (auto &[k, v] : ref) { assert(ks[i] == k && vs[i] == v); ++i; }
    std::cout << "200k random ops match std::map ok (final size " << d.size() << ")\n";
}

int main() {
    testPair();
    testBasics();
    testRandomAgainstStdMap();
    std::cout << "All tests passed.\n";
}
