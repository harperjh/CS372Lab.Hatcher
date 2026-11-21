//
//  File:   test_table.cpp
//  Purpose:
//  Test program for Table. Each test runs three times, once with each probe function:
//  findIndex (the default), quadraticProbe, and secondHash.
//  It checks normal use, collisions, deletions, and what happens when the table is full.
//
//  Build & run (Developer Command Prompt for VS):
//      cl /std:c++17 /EHsc test_table.cpp /Fe:test_table.exe
//      test_table.exe
//
#include "Table.hpp"

#include <cstdlib>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

// Record stored in the table
struct Record {
    int key = 0;
    std::string value;
};

using SmallTable = Table<Record, 11>;   // small prime size so collisions are easy to see
using BigTable = Table<Record, 811>;

static int g_passed = 0;
static int g_failed = 0;

#define CHECK(cond)                                                             \
    do {                                                                        \
        if (cond) ++g_passed;                                                   \
        else {                                                                  \
            ++g_failed;                                                         \
            std::cerr << "  FAIL line " << __LINE__ << ": " #cond << "\n";      \
        }                                                                       \
    } while (0)

// What we expect from one probe function on an 11-slot table
struct Strategy {
    std::string name;
    SmallTable::ProbeFunction small;      // probe function for Table<Record, 11>
    BigTable::ProbeFunction big;          // same probe function for Table<Record, 811>
    std::vector<std::size_t> slots;       // where keys 0, 11, 22, 33 land (all hash to 0)
    std::vector<std::size_t> probeCounts; // slots examined to find each of them
    std::size_t sameHashCapacity;         // how many keys with the same hash fit
};

// Insert or find using the default argument when the strategy is findIndex,
// so the default value of the parameter is actually tested
static bool doInsert(SmallTable& t, const Strategy& s, const Record& r) {
    return s.name == "findIndex" ? t.insert(r) : t.insert(r, s.small);
}

static void doFind(const SmallTable& t, const Strategy& s, int key, bool& found, Record& r) {
    if (s.name == "findIndex") t.find(key, found, r);
    else t.find(key, found, r, s.small);
}

// ---------------------------------------------------------------------------
// Basic insert / find / update on an empty table
// ---------------------------------------------------------------------------
static void testBasic(const Strategy& s) {
    SmallTable t;
    bool found = true;
    Record r;

    CHECK(t.size() == 0);
    doFind(t, s, 5, found, r);
    CHECK(!found);                                 // empty table

    CHECK(doInsert(t, s, {5, "five"}));
    CHECK(doInsert(t, s, {9, "nine"}));
    CHECK(t.size() == 2);
    CHECK(t.keyAt(5) == 5);                        // no collision: key goes to its home slot
    CHECK(t.keyAt(9) == 9);

    doFind(t, s, 5, found, r);
    CHECK(found && r.value == "five");
    CHECK(t.lastProbeCount() == 1);                // found on the first try

    CHECK(doInsert(t, s, {5, "FIVE"}));            // same key replaces the record
    CHECK(t.size() == 2);
    doFind(t, s, 5, found, r);
    CHECK(found && r.value == "FIVE");

    doFind(t, s, 6, found, r);
    CHECK(!found);
    CHECK(t.isPresent(9, s.small));
    CHECK(!t.isPresent(6, s.small));
}

// ---------------------------------------------------------------------------
// Collisions: 0, 11, 22, 33 all hash to slot 0 in an 11-slot table
// ---------------------------------------------------------------------------
static void testCollisions(const Strategy& s) {
    SmallTable t;
    const std::vector<int> keys = {0, 11, 22, 33};
    for (int k : keys) CHECK(doInsert(t, s, {k, "v" + std::to_string(k)}));
    CHECK(t.size() == 4);

    std::cout << "  after inserting 0, 11, 22, 33 (all hash to 0):\n";
    t.print();

    for (std::size_t n = 0; n < keys.size(); ++n) {
        CHECK(t.keyAt(s.slots[n]) == keys[n]);     // landed where this probe sequence says
        bool found = false;
        Record r;
        doFind(t, s, keys[n], found, r);
        CHECK(found && r.value == "v" + std::to_string(keys[n]));
        CHECK(t.lastProbeCount() == s.probeCounts[n]);
    }

    // Missing key with the same hash: probe walks the chain, then stops at a never-used slot
    bool found = true;
    Record r;
    doFind(t, s, 44, found, r);
    CHECK(!found);
}

// ---------------------------------------------------------------------------
// Removing from the middle of a collision chain
// ---------------------------------------------------------------------------
static void testRemove(const Strategy& s) {
    SmallTable t;
    for (int k : {0, 11, 22, 33}) doInsert(t, s, {k, "v"});

    t.remove(11, s.small);
    CHECK(t.size() == 3);
    CHECK(t.keyAt(s.slots[1]) == SmallTable::PREVIOUSLY_USED);
    CHECK(!t.isPresent(11, s.small));

    // Keys after the removed one must still be found (search goes past "previously used")
    CHECK(t.isPresent(22, s.small));
    CHECK(t.isPresent(33, s.small));

    std::cout << "  after removing 11 (xx = previously used):\n";
    t.print();

    // Re-inserting 11 reuses its old slot
    CHECK(doInsert(t, s, {11, "back"}));
    CHECK(t.keyAt(s.slots[1]) == 11);
    CHECK(t.size() == 4);

    t.remove(99, s.small);                          // removing a missing key does nothing
    CHECK(t.size() == 4);
}

// ---------------------------------------------------------------------------
// Full table: 11 keys with different hashes fill all 11 slots
// ---------------------------------------------------------------------------
static void testFull(const Strategy& s) {
    SmallTable t;
    for (int k = 0; k < 11; ++k) CHECK(doInsert(t, s, {k, "v"}));
    CHECK(t.size() == t.capacity());

    std::cout << "  full table:\n";
    t.print();

    CHECK(!doInsert(t, s, {11, "no room"}));       // insert fails, table unchanged
    CHECK(!doInsert(t, s, {50, "no room"}));
    CHECK(t.size() == 11);

    bool found = true;
    Record r;
    doFind(t, s, 50, found, r);                     // search of a full table still stops
    CHECK(!found);
    CHECK(t.lastProbeCount() == 11);                // ...after checking every slot

    CHECK(doInsert(t, s, {3, "updated"}));          // replacing an existing key still works
    doFind(t, s, 3, found, r);
    CHECK(found && r.value == "updated");

    t.remove(4, s.small);                           // after a removal there is room again
    CHECK(doInsert(t, s, {50, "fits now"}));
    CHECK(t.isPresent(50, s.small));
    CHECK(t.size() == 11);
}

// ---------------------------------------------------------------------------
// Worst case: every key hashes to slot 0
// ---------------------------------------------------------------------------
static void testSameHashUntilFull(const Strategy& s) {
    SmallTable t;
    std::size_t inserted = 0;
    for (int n = 0; n < 11; ++n) {
        if (doInsert(t, s, {n * 11, "v"})) ++inserted;
    }
    CHECK(inserted == s.sameHashCapacity);
    CHECK(t.size() == s.sameHashCapacity);
    std::cout << "  inserting 0, 11, 22, ... 110 (all hash to 0): " << inserted
              << " of 11 fit\n";
    t.print();

    if (s.sameHashCapacity < t.capacity()) {
        // Quadratic probing only reaches about half the slots from one home slot.
        // The table still has room, so a key with a different home slot fits.
        CHECK(t.size() < t.capacity());
        CHECK(doInsert(t, s, {2, "different hash"}));
    }
}

// ---------------------------------------------------------------------------
// Random operations on an 811-slot table, compared against std::map
// ---------------------------------------------------------------------------
static void testRandom(const Strategy& s) {
    BigTable t;
    std::map<int, std::string> reference;
    std::mt19937 rng(372);
    std::uniform_int_distribution<int> keyDist(0, 5000);
    std::uniform_int_distribution<int> opDist(0, 2);
    bool ok = true;

    for (int step = 0; step < 50000; ++step) {
        const int key = keyDist(rng);
        const int op = opDist(rng);
        if (op == 0 && reference.size() < 400) {    // keep load under half for quadratic probing
            const std::string value = std::to_string(step);
            if (!t.insert({key, value}, s.big)) ok = false;
            reference[key] = value;
        } else if (op == 1) {
            t.remove(key, s.big);
            reference.erase(key);
        } else {
            bool found = false;
            Record r;
            t.find(key, found, r, s.big);
            const auto it = reference.find(key);
            if (found != (it != reference.end())) ok = false;
            if (found && r.value != it->second) ok = false;
        }
        if (t.size() != reference.size()) ok = false;
    }
    CHECK(ok);
}

// ---------------------------------------------------------------------------
// Using a different probe for find than for insert does not work
// ---------------------------------------------------------------------------
static void testMismatchedProbe() {
    SmallTable t;
    for (int k : {0, 11, 22}) t.insert({k, "v"}, &SmallTable::secondHash);
    CHECK(t.isPresent(22, &SmallTable::secondHash));
    CHECK(!t.isPresent(22));          // linear probe looks at slots 0, 1 and stops: 22 is in slot 6
}

int main() {
    const std::vector<Strategy> strategies = {
        // linear:    slots 0, 1, 2, 3
        {"findIndex", &SmallTable::findIndex, &BigTable::findIndex,
         {0, 1, 2, 3}, {1, 2, 3, 4}, 11},
        // quadratic: slots 0, 0+1, 0+4, 0+9; only 6 different slots reachable from slot 0
        {"quadraticProbe", &SmallTable::quadraticProbe, &BigTable::quadraticProbe,
         {0, 1, 4, 9}, {1, 2, 3, 4}, 6},
        // double hashing: step = 7 - (key mod 7) -> 0:7, 11:3, 22:6, 33:2
        {"secondHash", &SmallTable::secondHash, &BigTable::secondHash,
         {0, 3, 6, 2}, {1, 2, 2, 2}, 11},
    };

    for (const Strategy& s : strategies) {
        const int before = g_failed;
        std::cout << "==== " << s.name
                  << (s.name == "findIndex" ? " (default argument)" : "") << " ====\n";
        testBasic(s);
        testCollisions(s);
        testRemove(s);
        testFull(s);
        testSameHashUntilFull(s);
        testRandom(s);
        std::cout << (g_failed == before ? "  PASS " : "  FAIL ") << s.name << "\n\n";
    }

    testMismatchedProbe();

    std::cout << g_passed << " checks passed, " << g_failed << " failed\n";
    return g_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
