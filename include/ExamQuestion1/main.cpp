#include <iostream>
#include <string>
#include "PriorityQueue.hpp"

int main() {
    PriorityQueue<std::string> pq;

    pq.push("homework", 6);
    pq.push("dinner", 5);
    pq.push("game", 1);             // lowest: fast path to the back
    pq.push("work", 7);
    pq.push("gym", 4);
    pq.push("school", 7);           // same priority as "work": should come after it
    pq.push("feed dog", 8);         // highest: goes to the very front

    std::cout << "Size: " << pq.size() << "\n";
    std::cout << "Popping in priority order:\n";
    while (!pq.empty()) {
        std::cout << "  " << pq.top() << "\n";
        pq.pop();
    }

    // Accessing an empty queue throws.
    try {
        pq.top();
    } catch (const std::out_of_range &e) {
        std::cout << "Caught: " << e.what() << "\n";
    }

    // Priority must be positive.
    try {
        pq.push("bad", 0);
    } catch (const std::invalid_argument &e) {
        std::cout << "Caught: " << e.what() << "\n";
    }

    // top() returns a reference, so the front item can be modified in place.
    PriorityQueue<int> nums;
    nums.push(10, 1);
    nums.push(20, 4);
    nums.top() = 99;
    std::cout << "Top after edit: " << nums.top() << "\n";

    return 0;
}
