//
//  File:   Table.hpp
//  Purpose:
//  Hash table using open addressing. RecordType must have a public `int key`
//  member (keys must be >= 0) and a default constructor.
//
//  The probe function (how to find the next hole) can be chosen per call:
//      findIndex       linear probing      i = (h + k) % CAPACITY
//      quadraticProbe  quadratic probing   i = (h + k*k) % CAPACITY
//      secondHash      double hashing      i = (h + k*h2) % CAPACITY, h2(x) = 7 - (x mod 7)
//  Use the same probe function for insert, find and remove on a given table.
//  CAPACITY should be a prime other than 7 so every probe sequence works.
//
#pragma once

#include <cassert>
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>

template <class RecordType, std::size_t CAPACITY = 811>
class Table {
public:
    // Probe function: given a key, sets found and the index of the key,
    // or (if not found) the index of the hole where it should go.
    using ProbeFunction = std::function<void(const Table&, int, bool&, std::size_t&)>;

    // index returned by a probe when there is no hole on the probe sequence
    static constexpr std::size_t NO_SLOT = CAPACITY;

    // Start with every slot marked never used
    Table() : used(0), probes(0) {
        for (std::size_t i = 0; i < CAPACITY; ++i) data[i].key = NEVER_USED;
    }

    // Insert or replace entry; returns false if no hole could be found
    bool insert(const RecordType& entry, ProbeFunction hashingFunction = &Table::findIndex) {
        assert(entry.key >= 0);
        bool found;
        std::size_t index;
        hashingFunction(*this, entry.key, found, index);
        if (found) {
            data[index] = entry;
            return true;
        }
        if (index == NO_SLOT) return false;
        data[index] = entry;
        ++used;
        return true;
    }

    // Remove the record with this key, if present (slot becomes previously used)
    void remove(int key, ProbeFunction hashingFunction = &Table::findIndex) {
        assert(key >= 0);
        bool found;
        std::size_t index;
        hashingFunction(*this, key, found, index);
        if (found) {
            data[index].key = PREVIOUSLY_USED;
            --used;
        }
    }

    // True if a record with this key is in the table
    bool isPresent(int key, ProbeFunction hashingFunction = &Table::findIndex) const {
        assert(key >= 0);
        bool found;
        std::size_t index;
        hashingFunction(*this, key, found, index);
        return found;
    }

    // Look up key; sets found and copies the record into result if found
    void find(int key, bool& found, RecordType& result,
              ProbeFunction hashingFunction = &Table::findIndex) const {
        assert(key >= 0);
        std::size_t index;
        hashingFunction(*this, key, found, index);
        if (found) result = data[index];
    }

    // Number of records in the table
    std::size_t size() const { return used; }

    // Number of slots in the table
    static constexpr std::size_t capacity() { return CAPACITY; }

    // ---------------- probe functions ----------------

    // Linear probing: try h, h+1, h+2, ...
    void findIndex(int key, bool& found, std::size_t& index) const {
        std::size_t i = hash(key);
        std::size_t count = 0;
        std::size_t firstHole = NO_SLOT;
        while (count < CAPACITY && data[i].key != NEVER_USED && data[i].key != key) {
            if (data[i].key == PREVIOUSLY_USED && firstHole == NO_SLOT) firstHole = i;
            ++count;
            i = (i + 1) % CAPACITY;
        }
        finishProbe(key, i, count, firstHole, found, index);
    }

    // Quadratic probing: try h, h+1, h+4, h+9, ...
    void quadraticProbe(int key, bool& found, std::size_t& index) const {
        const std::size_t h = hash(key);
        std::size_t i = h;
        std::size_t count = 0;
        std::size_t firstHole = NO_SLOT;
        while (count < CAPACITY && data[i].key != NEVER_USED && data[i].key != key) {
            if (data[i].key == PREVIOUSLY_USED && firstHole == NO_SLOT) firstHole = i;
            ++count;
            i = (h + count * count) % CAPACITY;
        }
        finishProbe(key, i, count, firstHole, found, index);
    }

    // Double hashing: try h, h+h2, h+2*h2, ... with h2(x) = 7 - (x mod 7)
    void secondHash(int key, bool& found, std::size_t& index) const {
        const std::size_t h = hash(key);
        const std::size_t step = hash2(key);
        std::size_t i = h;
        std::size_t count = 0;
        std::size_t firstHole = NO_SLOT;
        while (count < CAPACITY && data[i].key != NEVER_USED && data[i].key != key) {
            if (data[i].key == PREVIOUSLY_USED && firstHole == NO_SLOT) firstHole = i;
            ++count;
            i = (h + count * step) % CAPACITY;
        }
        finishProbe(key, i, count, firstHole, found, index);
    }

    // ---------------- inspection (for testing) ----------------

    // Key stored in slot i, or NEVER_USED / PREVIOUSLY_USED
    int keyAt(std::size_t i) const { return data[i].key; }

    // Number of slots examined by the most recent probe
    std::size_t lastProbeCount() const { return probes; }

    // Print every slot: key, "--" for never used, "xx" for previously used
    void print(std::ostream& out = std::cout) const {
        for (std::size_t i = 0; i < CAPACITY; ++i) {
            out << "[" << std::setw(2) << i << ":";
            if (data[i].key == NEVER_USED) out << std::setw(4) << "--";
            else if (data[i].key == PREVIOUSLY_USED) out << std::setw(4) << "xx";
            else out << std::setw(4) << data[i].key;
            out << "] ";
            if (i % 8 == 7) out << "\n";
        }
        out << "\n";
    }

    static constexpr int NEVER_USED = -1;
    static constexpr int PREVIOUSLY_USED = -2;

private:
    RecordType data[CAPACITY];
    std::size_t used;
    mutable std::size_t probes;

    // Primary hash
    static std::size_t hash(int key) { return static_cast<std::size_t>(key) % CAPACITY; }

    // Second hash for double hashing (always 1..7, never 0)
    static std::size_t hash2(int key) { return 7 - (static_cast<std::size_t>(key) % 7); }

    // Shared ending for all probes: work out found/index from where the loop stopped
    void finishProbe(int key, std::size_t i, std::size_t count, std::size_t firstHole,
                     bool& found, std::size_t& index) const {
        if (count == CAPACITY) {                 // tried CAPACITY slots, no key, no never-used slot
            probes = count;
            found = false;
            index = firstHole;                   // a previously-used slot, or NO_SLOT
            return;
        }
        probes = count + 1;
        found = (data[i].key == key);
        if (found) index = i;
        else index = (firstHole != NO_SLOT) ? firstHole : i;   // reuse a deleted slot if we passed one
    }
};
