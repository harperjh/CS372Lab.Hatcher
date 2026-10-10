#pragma once
#include <cstddef>      // std::size_t
#include <utility>      // std::pair
#include <stdexcept>
#include "List.hpp"

// Priority queue ADT built on top of List<T>.
//
// Invariant: items are stored in the list in order of priority, highest
//            first, so the head of the list is always the top of the queue.
//            Items with equal priority stay in arrival order (FIFO).
//
// Value set: pairs of (priority, item). The priority is the pair's key
//            (first), the item is the value (second).
template <typename T>
class PriorityQueue {
private:
    typedef std::pair<unsigned int, T> Entry;   // first = priority (key)
    List<Entry> pqList;                         // head = highest priority

public:
    bool empty() const { return pqList.empty(); }
    std::size_t size() const { return pqList.size(); }

    // Inserts newItem so the list stays sorted from highest to lowest priority.
    //
    // Big-O: O(n) worst case, O(1) best case.
    //   - If the new item's priority is no higher than the last item's, it
    //     belongs at the back, and push_back is O(1) thanks to the tail pointer.
    //   - Otherwise we must walk from the head until we find the first item
    //     with a strictly lower priority. A linked list has no random access,
    //     so even though it is sorted we cannot binary search it; finding the
    //     spot takes up to n steps. The actual splice is O(1).
    void push(T newItem, unsigned int itemPriority) {
        if (itemPriority == 0) {
            throw std::invalid_argument("push(): priority must be a positive integer");
        }
        Entry entry(itemPriority, newItem);

        // Fast path: belongs at the back (also covers the empty queue).
        if (pqList.empty() || itemPriority <= pqList.back().first) {
            pqList.push_back(entry);
            return;
        }

        // Go in front of the first item with a strictly lower priority.
        // Using "<" (not "<=") keeps equal priorities in FIFO order.
        pqList.insert_before_first(
            [itemPriority](const Entry& existing) {
                return existing.first < itemPriority;
            },
            entry);
    }

    // Returns the highest-priority item.
    // Big-O: O(1). The invariant guarantees it is at the head of the list.
    T& top() {
        if (pqList.empty()) {
            throw std::out_of_range("top(): priority queue is empty");
        }
        return pqList.front().second;
    }

    const T& top() const {
        if (pqList.empty()) {
            throw std::out_of_range("top(): priority queue is empty");
        }
        return pqList.front().second;
    }

    // Removes the highest-priority item.
    // Big-O: O(1). It is the head node, and pop_front just unlinks it.
    void pop() {
        if (pqList.empty()) {
            throw std::out_of_range("pop(): priority queue is empty");
        }
        pqList.pop_front();
    }
};

// ===================== Why this is not the most efficient design =====================
//
// Cost of each operation (n = number of items in the queue):
//
//     operation   sorted linked list (this class)   binary heap (std::priority_queue)
//     push        O(n)                              O(log n)
//     top         O(1)                              O(1)
//     pop         O(1)                              O(log n)
//
// 1. push is the bottleneck. Keeping the whole list sorted means every insert
//    may scan the entire list to find its slot, and a linked list only allows
//    sequential access, so the scan is linear. Pushing n items costs
//    1 + 2 + ... + n = O(n^2) in the worst case; this is just insertion sort.
//
// 2. Full sorting is more order than the ADT needs. A priority queue only ever
//    exposes its single highest item. A binary heap keeps a much weaker rule
//    (each parent's priority >= its children's), so a new item only travels
//    up one leaf-to-root path of height log n: push is O(log n).
//
// 3. Over a whole workload of n pushes and n pops:
//        sorted list:  O(n^2) + O(n)          = O(n^2)
//        binary heap:  O(n log n) + O(n log n) = O(n log n)
//    Our O(1) pop beats the heap's O(log n), but that does not make up for
//    the O(n) push. For n = 1,000,000 the gap is roughly n / log n, about
//    50,000 times more work for the list.
//
// 4. Constant factors also favor the heap. Each list node is a separate
//    allocation with two extra pointers (prev, next), scattered in memory,
//    so walking the list misses the cache often. A heap lives in one
//    contiguous array and finds children by index (2i+1, 2i+2), no pointers.
//
// The sorted list is simple and fine for small queues or when pops far
// outnumber pushes, but in general an array-based binary heap is the
// better way to build a priority queue.
// =====================================================================================
