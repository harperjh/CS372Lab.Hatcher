#pragma once
#include <iostream>        
#include <stdexcept>      
#include "../List/List.hpp"

template <typename T>
class Queue {
private:
    List<T> queueList;

public:
    Queue() {}
    Queue(Queue& rhs) {}
    // Destructor
    ~Queue() {}

    // Returns true if the queue holds no items.
    bool empty() { return queueList.empty(); }

    // Adds an item to the back of the queue (stored at the List's front).
    void push(T data) { queueList.push_front(data); }

    T front() { return queueList.back(); }
    // Returns the item at the back of the queue: the most recently pushed
    T back() { return queueList.front(); }

    // Removes the item at the front of the queue (the oldest one).
    void pop() { queueList.pop_back(); }

    // Calls doIt on every item, from the List's front to its back
    void traverse(void (*doIt)(T& data)) {
        queueList.traverse(doIt);
    };
};