#pragma once
#include "List.hpp"
#include <functional>
#include <iostream>
// A circular list built by inheriting from List<T>. Traversal wraps from the
// tail back to the head, stopping once we return to the starting node.
//
// List<T> owns its nodes through std::unique_ptr (head and each node->next).
// CircularList only *walks* the nodes, so it uses non-owning raw Node*
// obtained with .get(); it never owns, creates or deletes a node itself.
// The wrap-around is done during traversal instead of storing a real link
// from tail back to head: a unique_ptr cycle would mean every node owns the
// next one forever and nothing could ever be freed.
template <typename T>
class CircularList : public List<T> {
public:
using List<T>::List;// reuse List<T>'s constructors

void traverse(std::function<void(T &data)> doIt) {
// Traverse starting at the front of the list (index 0).
traverse(0, doIt);
}
// Start at startIndex, wrap from the tail back to the head, and stop once
// we return to the starting node.
void traverse(int startIndex, std::function<void(T &data)> doIt) {
if (this->empty()) {
std::cerr << "traverse(): cannot traverse an empty circular list." << std::endl;
return;
}
typename List<T>::Node *startNode = nodeAt(startIndex);
typename List<T>::Node *current = startNode;
do {
doIt(current->data);
// wrap back to head instead of stopping at nullptr
current = (current->next != nullptr) ? current->next.get() : this->head.get();
} while (current != startNode);
}

private:
// Walks from head to find the node at a given logical index.
typename List<T>::Node* nodeAt(int index) {
int n = static_cast<int>(this->size());
int normalized = ((index % n) + n) % n;
typename List<T>::Node *node = this->head.get();
for (int i = 0; i < normalized; i++) {
node = node->next.get();
}
return node;
}
};
