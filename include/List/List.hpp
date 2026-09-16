#pragma once
#include <functional>
#include <iostream>
#include <cstddef>

template <typename T>
class List {
protected:
	// protected (not private) so CircularList can walk the raw nodes
	class Node {
	public:
		T data;
		Node* prev = nullptr;
		Node* next = nullptr;
	};
	Node* head;
	Node* tail;
	size_t listSize;
	// frees every node and resets the list to empty
	void deleteListContents() {
		Node* current = head;
		Node* temp = nullptr;
		while (current != nullptr) {
			temp = current->next;
			delete current;
			current = temp;
		}
		head = nullptr;
		tail = nullptr;
		listSize = 0;
	}

public:
	// copy constructor: deep-copies every node from rhs, in order
	List() : head(nullptr), tail(nullptr), listSize(0) {}

	explicit List(T newData) : head(nullptr), tail(nullptr), listSize(0) {
		push_back(newData);
	}

	List(const List &rhs) : head(nullptr), tail(nullptr), listSize(0) {
		for (Node *curr = rhs.head; curr != nullptr; curr = curr->next) {
			push_back(curr->data);
		}
	}
	// copy assignment: guards against self-assignment, then deep-copies
	List & operator=(const List &rhs) {
		if (this == &rhs) {
			return *this;
		}
		deleteListContents();
		for (Node *curr = rhs.head; curr != nullptr; curr = curr->next) {
			push_back(curr->data);
		}
		return *this;
	}
	// move constructor: steals rhs's nodes instead of copying them
	List(List &&rhs) noexcept
		: head(rhs.head), tail(rhs.tail), listSize(rhs.listSize) {
		rhs.head = nullptr;
		rhs.tail = nullptr;
		rhs.listSize = 0;
	}

	List & operator=(List &&rhs) noexcept {
		if (this == &rhs) {
			return *this;
		}
		deleteListContents();
		head = rhs.head;
		tail = rhs.tail;
		listSize = rhs.listSize;
		rhs.head = nullptr;
		rhs.tail = nullptr;
		rhs.listSize = 0;
		return *this;
	}

	virtual ~List() {
		deleteListContents();
	}

	bool empty() const {
		return (head == nullptr);
	}

	size_t size() const {
		return listSize;
	}
	// insert at the front of the list
	void push_front(T data) {
		Node *newNode = new Node();
		newNode->data = data;
		if (empty()) {
			head = newNode;
			tail = newNode;
		}
		else {
			newNode->next = head;
			head->prev = newNode;
			head = newNode;
		}
		listSize++;
	}
	// insert at the back of the list
	void push_back(T data) {
		Node* newNode = new Node();
		newNode->data = data;
		if (empty()) {
			head = newNode;
			tail = newNode;
		}
		else {
			newNode->prev = tail;
			tail->next = newNode;
			tail = newNode;
		}
		listSize++;
	}

	T &front() const {
		return(head->data);
	}

	T &back() const {
		return (tail->data);
	}
	// remove the last element; also clears head if the list becomes empty
	void pop_back() {
		if (!empty()) {
			Node *lastNode = tail;
			tail = lastNode->prev;
			if (tail != nullptr) {
				tail->next = nullptr;
			}
			else {
				head = nullptr;
			}
			delete lastNode;
			listSize--;
		}
		else {
			std::cerr << "pop_back(): Attempt to pop from empty list. " << std::endl;
		}
	}
	// remove the first element; also clears tail if the list becomes empty
	void pop_front() {
		if (!empty()) {
			Node *firstNode = head;
			head = firstNode->next;
			if (head != nullptr) {
				head->prev = nullptr;
			}
			else {
				tail = nullptr;
			}
			delete firstNode;
			listSize--;
		}
		else {
			std::cerr << "pop_front(): Attempt to pop from empty list. " << std::endl;
		}
	}

	void traverse(std::function<void(T &data)> doIt) {
		Node *current = head;
		while (current != nullptr) {
			doIt(current->data);
			current = current->next;
		}
	}
};
