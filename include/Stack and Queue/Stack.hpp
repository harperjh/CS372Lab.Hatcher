#pragma once
#include <iostream>        
#include <stdexcept>       
#include "../List/List.hpp"

template <typename T>
class Stack {
private:
    // The underlying storage. The top of the stack is the List's front.
    List<T> stackList;

public:
    Stack() {}
    Stack(Stack& rhs) {}

    // Destructor
    ~Stack() {}

    // Returns true if the stack holds no items.
    bool empty() { return stackList.empty(); }
    // Places an item on top of the stack.
    void push(T data) { stackList.push_front(data); }
    // Removes the item on top of the stack.
    void pop() { return stackList.pop_front(); }
    // This plays the role usually called top().
    T front() { return stackList.front(); }

    // Returns the item at the bottom of the stack (the first one pushed).
    T back() { return stackList.back(); }

    // Calls doIt on every item, from the top of the stack to the bottom.
    void traverse(void (*doIt)(T& data)) {
        stackList.traverse(doIt);
    };
};