//
// File:    TreeAlgorithms.h
// Purpose:
//  Stand-alone (non-member) recursive algorithms on Tree:
//    * levelorder - level (breadth-first) traversal
//    * prune      - copy of a tree with every leaf removed
//  Both use only Tree's public interface (isEmpty, root, left, right and the
//  three-argument constructor), so they do not need to be members.
//
#pragma once

#include <queue>
#include "Tree.h"

//
// Level traversal
// ---------------
// Visits the root (level 0), then every node on level 1, then every node on
// level 2, and so on.  Within a level, nodes are visited left to right.
//
// `visit` is any callable (function pointer, functor, lambda) that accepts
// the value stored in a node, exactly like the visitor passed to
// Tree::preorder/inorder/postorder.
//
// How it works: a std::queue holds the subtrees on the current level.  Each
// recursive call visits every node on that level and collects their non-empty
// children into a second queue, which is the next level.  The function then
// calls itself on the next level, and stops when a level is empty.
// Recursing once per LEVEL (not once per node) keeps the recursion depth
// equal to the height of the tree.
//
template <typename T, typename Compare, typename Visitor>
void levelorderHelper(std::queue<Tree<T, Compare>>& thisLevel, Visitor& visit) {
    if (thisLevel.empty()) return;                    // base case: no more levels

    std::queue<Tree<T, Compare>> nextLevel;
    while (!thisLevel.empty()) {
        Tree<T, Compare> node = thisLevel.front();    // O(1): copies one shared_ptr
        thisLevel.pop();
        visit(node.root());
        if (!node.left().isEmpty())  nextLevel.push(node.left());
        if (!node.right().isEmpty()) nextLevel.push(node.right());
    }
    levelorderHelper(nextLevel, visit);               // recurse on the next level
}

template <typename T, typename Compare, typename Visitor>
void levelorder(const Tree<T, Compare>& tree, Visitor visit) {
    std::queue<Tree<T, Compare>> firstLevel;
    if (!tree.isEmpty()) firstLevel.push(tree);
    levelorderHelper(firstLevel, visit);
}

//
// Prune
// -----
// Returns a new tree that is a copy of `tree` with every leaf (a node with no
// children) removed.  The original tree is not changed.
//
//   prune(empty)               = empty
//   prune(leaf)                = empty                  (drop the leaf)
//   prune((left, v, right))    = (prune(left), v, prune(right))
//
// The assignment text says "returns a reference to a new Tree".  The result
// is returned BY VALUE instead: the new tree is created inside the function,
// so a reference to it would dangle as soon as the function returned.
// Returning a Tree by value is cheap here (it moves one shared_ptr).
//
// Removing leaves keeps the binary-search-tree property (every remaining node
// keeps the same left/right relationship to its ancestors), and the new nodes
// get correct parent pointers because they are built with the three-argument
// Tree constructor.
//
template <typename T, typename Compare>
Tree<T, Compare> prune(const Tree<T, Compare>& tree) {
    if (tree.isEmpty()) return Tree<T, Compare>();
    Tree<T, Compare> lft = tree.left();
    Tree<T, Compare> rgt = tree.right();
    if (lft.isEmpty() && rgt.isEmpty()) return Tree<T, Compare>();   // leaf
    return Tree<T, Compare>(prune(lft), tree.root(), prune(rgt));
}
