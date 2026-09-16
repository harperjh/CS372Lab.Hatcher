#pragma once
#include "List.hpp"
#include <functional>
#include <iostream>
// A circular list built by inheriting from List<T>.Traversal wrap
template <typename T>
class CircularList : public List<T> {
public:
	using List<T>::List;// reuse List<T>'s constructors

	void traverse(std::function<void(T &data)> doIt) {
		// Traverse starting at the front of the list (index 0).
		traverse(0, doIt);
	}
	// tail back to the head, and stopping once we return to that node.
	void traverse(int startIndex, std::function<void(T &data)> doIt) {
		if (this->empty()) {
			std::cerr << "traverse(): cannot traverse an empty circular list." << std::endl;
			return;
		}
		typename List<T>::Node *startNode = nodeAt(startIndex);
		typename List<T>::Node *current = startNode;
		do {
			doIt(current->data);
			current = (current->next != nullptr) ? current->next : this->head;// wrap back to head instead of stopping at nullptr
		} while (current != startNode);
	}

private:
	// Walks from head to find the node at a given logical index.
	typename List<T>::Node* nodeAt(int index) {
		int n = static_cast<int>(this->size());
		int normalized = ((index % n) + n) % n;
		typename List<T>::Node *node = this->head;
		for (int i = 0; i < normalized; i++) {
			node = node->next;
		}
		return node;
	}
};
