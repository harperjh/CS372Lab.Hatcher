
#pragma once

#include <functional>
#include <iostream>
#include <cstddef>
#include <memory>
#include <utility>
#include <iterator>
#include <type_traits>

//
// Doubly linked list using smart pointers.
//
// Ownership model:
//   - head owns the first node (std::unique_ptr)
//   - each node->next owns the next node (std::unique_ptr)
//   - node->prev and tail are NON-owning observers (raw Node*)
//
// Every node therefore has exactly one owner, so there are no cycles,
// no leaks, and no manual delete anywhere in this class.
//

template <typename T>
class List {
protected:
    // protected (not private) so CircularList can walk the nodes
    class Node {
    public:
        T data;
        Node* prev = nullptr;          // non-owning back link
        std::unique_ptr<Node> next;    // owning forward link
    };

    std::unique_ptr<Node> head;
    Node* tail;                        // non-owning: the last node is owned by its predecessor (or head)
    size_t listSize;

    // Frees every node and resets the list to empty.
    // Released one node at a time on purpose: letting head's destructor
    // cascade down the chain would recurse once per node and could
    // overflow the stack on a long list.
    void deleteListContents() {
        while (head) {
            head = std::move(head->next);
        }

        tail = nullptr;
        listSize = 0;
    }

public:
    // Default constructor
    List() : head(nullptr), tail(nullptr), listSize(0) {}

    // Constructor with initial data
    explicit List(T newData)
        : head(nullptr), tail(nullptr), listSize(0) {
        push_back(newData);
    }

    // Copy constructor: deep-copies every node from rhs, in order
    List(const List& rhs)
        : head(nullptr), tail(nullptr), listSize(0) {

        for (Node* curr = rhs.head.get();
            curr != nullptr;
            curr = curr->next.get()) {
            push_back(curr->data);
        }
    }

    // Copy assignment: guards against self-assignment, then deep-copies
    List& operator=(const List& rhs) {
        if (this == &rhs) {
            return *this;
        }

        deleteListContents();

        for (Node* curr = rhs.head.get();
            curr != nullptr;
            curr = curr->next.get()) {
            push_back(curr->data);
        }

        return *this;
    }

    // Move constructor: takes ownership of rhs's nodes instead of copying them
    List(List&& rhs) noexcept
        : head(std::move(rhs.head)),
        tail(rhs.tail),
        listSize(rhs.listSize) {

        rhs.tail = nullptr;
        rhs.listSize = 0;
    }

    // Move assignment operator
    List& operator=(List&& rhs) noexcept {
        if (this == &rhs) {
            return *this;
        }

        deleteListContents();          // iterative free of our old nodes first

        head = std::move(rhs.head);
        tail = rhs.tail;
        listSize = rhs.listSize;

        rhs.tail = nullptr;
        rhs.listSize = 0;

        return *this;
    }

    // Destructor
    virtual ~List() {
        deleteListContents();
    }

    // Check whether the list is empty
    bool empty() const {
        return (head == nullptr);
    }

    // Return the number of elements
    size_t size() const {
        return listSize;
    }

    // Insert at the front of the list
    void push_front(T data) {
        auto newNode = std::make_unique<Node>();
        newNode->data = data;

        if (empty()) {
            tail = newNode.get();
            head = std::move(newNode);
        }
        else {
            head->prev = newNode.get();
            newNode->next = std::move(head);
            head = std::move(newNode);
        }

        listSize++;
    }

    // Insert at the back of the list
    void push_back(T data) {
        auto newNode = std::make_unique<Node>();
        newNode->data = data;

        if (empty()) {
            tail = newNode.get();
            head = std::move(newNode);
        }
        else {
            newNode->prev = tail;
            tail->next = std::move(newNode);
            tail = tail->next.get();
        }

        listSize++;
    }

    // Access the first element
    T& front() {
        return (head->data);
    }

    const T& front() const {
        return (head->data);
    }

    // Access the last element
    T& back() {
        return (tail->data);
    }

    const T& back() const {
        return (tail->data);
    }

    // Remove the last element; also clears head if the list becomes empty
    void pop_back() {
        if (!empty()) {
            if (head.get() == tail) {   // only one node
                head.reset();
                tail = nullptr;
            }
            else {
                tail = tail->prev;
                tail->next.reset();     // frees the old last node
            }

            listSize--;
        }
        else {
            std::cerr << "pop_back(): Attempt to pop from empty list. "
                << std::endl;
        }
    }

    // Remove the first element; also clears tail if the list becomes empty
    void pop_front() {
        if (!empty()) {
            head = std::move(head->next);   // old first node is freed here

            if (head != nullptr) {
                head->prev = nullptr;
            }
            else {
                tail = nullptr;
            }

            listSize--;
        }
        else {
            std::cerr << "pop_front(): Attempt to pop from empty list. "
                << std::endl;
        }
    }

    // --------------------------------------------------------
    // Iterators
    // --------------------------------------------------------
    // Forward iterators that walk the nodes via the non-owning Node* view
    // (next.get()), so iterating never touches ownership.

    template <bool IsConst>
    class BasicIterator {
        using NodePtr = std::conditional_t<IsConst, const Node*, Node*>;

        NodePtr curr;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;

        explicit BasicIterator(NodePtr n = nullptr) : curr(n) {}

        reference operator*() const {
            return curr->data;
        }

        pointer operator->() const {
            return &curr->data;
        }

        BasicIterator& operator++() {
            curr = curr->next.get();
            return *this;
        }

        BasicIterator operator++(int) {
            BasicIterator tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==(const BasicIterator& o) const {
            return curr == o.curr;
        }

        bool operator!=(const BasicIterator& o) const {
            return curr != o.curr;
        }
    };

    using iterator = BasicIterator<false>;
    using const_iterator = BasicIterator<true>;

    iterator begin() {
        return iterator(head.get());
    }

    iterator end() {
        return iterator(nullptr);
    }

    const_iterator begin() const {
        return const_iterator(head.get());
    }

    const_iterator end() const {
        return const_iterator(nullptr);
    }

    const_iterator cbegin() const {
        return const_iterator(head.get());
    }

    const_iterator cend() const {
        return const_iterator(nullptr);
    }

    // --------------------------------------------------------
    // Traversal
    // --------------------------------------------------------

    void traverse(std::function<void(T& data)> doIt) {
        for (Node* current = head.get();
            current != nullptr;
            current = current->next.get()) {
            doIt(current->data);
        }
    }

    // --------------------------------------------------------
    // Conditional Insertion
    // --------------------------------------------------------
    // Insert data in front of the first node for which
    // goesBefore(node's data) returns true.
    //
    // If no node qualifies, data is appended at the back.
    // One walk from head, so O(n) worst case;
    // the relinking itself is O(1).

    void insert_before_first(
        std::function<bool(const T& existing)> goesBefore,
        const T& data) {

        Node* current = head.get();

        while (current != nullptr && !goesBefore(current->data)) {
            current = current->next.get();
        }

        if (current == nullptr) {       // nothing qualified (or list empty)
            push_back(data);
            return;
        }

        if (current == head.get()) {    // new node becomes the first node
            push_front(data);
            return;
        }

        // General case: splice between before (= current->prev)
        // and current.
        //
        // before->next currently owns current; hand that
        // ownership to newNode, then make before->next own newNode.

        auto newNode = std::make_unique<Node>();
        newNode->data = data;

        Node* before = current->prev;

        newNode->prev = before;
        newNode->next = std::move(before->next);

        current->prev = newNode.get();
        before->next = std::move(newNode);

        listSize++;
    }
};
