// Purpose:
// Use C++ templates, STL classes and functions, and smart pointers to
// implement a binary search tree.
//
// A binary search tree is defined as being either empty, or a 3-tuple
// (left.subtree, value, right.subtree).  Which values go left is decided by
// the tree's comparator `Compare`: a value x goes to the left of y when
// Compare(x, y) is true.  With the default, std::greater<T>, LARGER values go
// to the left and an inorder traversal visits values in DESCENDING order.
// Use Tree<T, std::less<T>> for the textbook "smaller on the left" ordering.
//
// Note: We are strongly encouraged by the C++ Core Guidelines to strive
// for immutability in our data structures.  This means that we should not
// modify the tree in place, but rather create a new tree that is a modified
// version of the original tree.  This is a bit of a pain, but it is the
// right thing to do.  It allows us to reason about our code more easily, and
// it allows us to use the tree in a functional programming style.
//
// The downside of immutability is that there is a lot of copying of data.
// This is mitigated by the use of smart pointers, which allow us to share
// ownership.  But even so, we should be careful about how we use this class
// to avoid unnecessary copies.
//
// ---------------------------------------------------------------------------
// Parent pointers
// ---------------------------------------------------------------------------
// Every node also records its parent so we can walk from any node back up to
// the root.  The root's parent is nullptr.
//
//  * The parent link is a std::weak_ptr, not a shared_ptr.  A parent already
//    owns its children through shared_ptr; if children also owned their parent
//    we would have a reference cycle and no node would ever be freed.
//
//  * The link is `mutable` because nodes are stored as shared_ptr<const Node>.
//    A child has to be created before its parent (the parent's constructor
//    takes the finished children), so the child's parent can only be filled in
//    afterwards.  The value and the child links stay immutable.
//
//  * A node can have only ONE parent.  The original design shares unchanged
//    subtrees between the old and the new tree after insert(); a shared node
//    would then belong to two parents and one of the two trees would see a
//    wrong parent.  So when we build a node whose child already belongs to a
//    tree that is still alive, we give the new node its own copy of that
//    child subtree (see adopt()).  A child whose old parent is gone (the usual
//    `t = t.insert(x)` case once the old t is released, or a brand-new
//    subtree) is reused without copying.
//
// ---------------------------------------------------------------------------
// Known limitations (documented, not fixed -- see docs/Tree_AI_Review.md)
// ---------------------------------------------------------------------------
//  * The tree is not balanced.  Sorted input gives a list-shaped tree, so
//    insert/member/find/traversals are O(n) and recurse O(n) deep.
//  * Because of the one-parent rule above, insert() on a tree that is still
//    referenced elsewhere copies the off-path subtrees: O(n) instead of
//    O(log n).  This is the price of combining parent links with persistence.
//  * Building new trees from the SAME shared subtree on several threads at
//    once is not thread-safe (the mutable parent link is written).  Reading
//    an already-built tree from many threads is fine.
//
#pragma once

#include <memory>
#include <functional>
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

template<typename T, typename Compare = std::greater<T>>
class Tree
{
    // The inner struct Node represents one node of the tree.  This defines the
    // underlying structure implied by the mathematical definition of the Tree
    // ADT
    //
    struct Node
    {
        Node(std::shared_ptr<const Node> lft
             , T val
             , std::shared_ptr<const Node> rgt)
        : _lft(std::move(lft)), _val(std::move(val)), _rgt(std::move(rgt))
        {}

        std::shared_ptr<const Node> _lft;
        T _val;
        std::shared_ptr<const Node> _rgt;
        std::size_t _size = 1;                       // nodes in this subtree
        mutable std::weak_ptr<const Node> _parent;   // empty for the root
    };

    //
    // And this private constructor defines how we keep track of the root of the
    // tree while not exposing that information to clients of this class.
    //
    explicit Tree(std::shared_ptr<const Node> node)
      : _root(std::move(node)) {}

    // The ordering is part of the tree's type, so every operation on a given
    // tree always uses the same comparator.
    static bool goesLeftOf(const T& x, const T& y) { return Compare{}(x, y); }

    static std::size_t sizeOf(const std::shared_ptr<const Node>& n) {
        return n ? n->_size : 0;
    }

    // Deep-copies a subtree, wiring up parent links inside the copy.
    // The copy's root has no parent yet; the caller attaches it.
    static std::shared_ptr<const Node> clone(const std::shared_ptr<const Node>& n) {
        if (!n) return nullptr;
        return makeNode(clone(n->_lft), n->_val, clone(n->_rgt));
    }

    // Returns a subtree that the node being built may claim as its child:
    // the subtree itself if nobody else is its parent, otherwise a copy.
    static std::shared_ptr<const Node> adopt(const std::shared_ptr<const Node>& child) {
        if (!child) return nullptr;
        if (child->_parent.expired()) return child;  // free: reuse, no copy
        return clone(child);                         // taken: copy
    }

    // Single place where nodes are created: builds the node, records its
    // subtree size, and points each child's parent link back at it.  The left
    // child is claimed before the right one is checked, so Tree(a, v, a) gets
    // two separate children.
    static std::shared_ptr<const Node> makeNode(const std::shared_ptr<const Node>& lft,
                                                const T& val,
                                                const std::shared_ptr<const Node>& rgt) {
        auto n = std::make_shared<Node>(adopt(lft), val, nullptr);
        if (n->_lft) n->_lft->_parent = n;
        n->_rgt = adopt(rgt);
        if (n->_rgt) n->_rgt->_parent = n;
        n->_size = 1 + sizeOf(n->_lft) + sizeOf(n->_rgt);
        return n;   // converts to shared_ptr<const Node>; frozen from here on
    }

    // Defined behavior (an exception) instead of a null dereference when an
    // operation needs a node but the tree is empty -- asserts vanish in
    // release builds.
    void requireNonEmpty(const char* what) const {
        if (isEmpty()) throw std::out_of_range(std::string("Tree::") + what + " on an empty tree");
    }

public:
    //
    // The first two public constructors provide the protocol for creating a
    // Tree per the mathematical definition of the Tree ADT.
    //
    Tree(): _root(nullptr) {}
    Tree(Tree const & other) = default;
    Tree & operator=(Tree const & other) = default;
    ~Tree() = default;
    Tree(Tree && other) = default;
    Tree & operator=(Tree && other) = default;
    //

    Tree(const Tree& lft, const T& val, const Tree& rgt)
    : _root(makeNode(lft._root, val, rgt._root))
    {}

    //
    // We add an additional constructor that we use to construct a Tree from
    // an initializer list.
    //
    Tree(std::initializer_list<T> init) {
        Tree t;
        for (const T& v: init) {
            t = t.insert(v);
        }
        _root = t._root;
    }

    //
    // The next portion of the protocol allows the client to query the state
    // of the Tree.   Again, note how we avoid exposing the state to the client.
    //
    bool isEmpty() const { return !_root; }

    // O(1): each node stores the size of its subtree.
    std::size_t size() const { return sizeOf(_root); }

    const T &root() const {
        requireNonEmpty("root()");
        return _root->_val;
    }

    Tree left() const {
        requireNonEmpty("left()");
        return Tree(_root->_lft);
    }

    Tree right() const {
        requireNonEmpty("right()");
        return Tree(_root->_rgt);
    }

    //
    // Parent navigation.
    // hasParent() is false for the root of a tree (its parent is nullptr).
    // parent() returns the subtree rooted at this node's parent, or an empty
    // Tree for the root.
    //
    bool hasParent() const {
        requireNonEmpty("hasParent()");
        return !_root->_parent.expired();
    }

    Tree parent() const {
        requireNonEmpty("parent()");
        return Tree(_root->_parent.lock());
    }

    // Values on the path from this node up to the root, this node first.
    std::vector<T> pathToRoot() const {
        std::vector<T> path;
        for (auto n = _root; n; n = n->_parent.lock()) {
            path.push_back(n->_val);
        }
        return path;
    }

    //
    // Now we manipulate the tree.
    // Note how insert operates... we don't try to "fix" an existing tree.
    // Rather we build a new tree, inserting the new value at the correct point.
    //
    // x goes into the left subtree when Compare(x, root) is true, into the
    // right subtree when Compare(root, x) is true, and is a duplicate (ignored)
    // when neither is true.
    //
    Tree insert(const T &x) const {
        if (isEmpty())
            return Tree(Tree(), x, Tree());
        const T& y = root();
        if (goesLeftOf(x, y))
            return Tree(left().insert(x), y, right());
        else if (goesLeftOf(y, x))
            return Tree(left(), y, right().insert(x));
        else
            return *this; // no duplicates
    }

    // Returns true if x is stored in the tree.
    bool member(const T &x) const {
        if (isEmpty()) {
            return false;
        }
        const T& y = root();
        if (goesLeftOf(x, y)) {
            return left().member(x);
        } else if (goesLeftOf(y, x)) {
            return right().member(x);
        } else {
            return true;
        }
    }

    //
    // This function finds a value in the tree, and returns true if it is found.
    // If it is found, the subtree where it was found (rooted at the matching
    // node) is returned in the second parameter; otherwise that parameter is
    // set to an empty tree.
    //
    bool find(const T &x, Tree &subtreeWhereFound) const {
        if (isEmpty()) {
            subtreeWhereFound = Tree();
            return false;
        }
        const T& y = root();
        if (goesLeftOf(x, y))
            return left().find(x, subtreeWhereFound);
        else if (goesLeftOf(y, x))
            return right().find(x, subtreeWhereFound);
        else {
            subtreeWhereFound = *this;
            return true;
        }
    }

    //
    // For each of traversal functions, we assume that the parameter is a
    // function pointer, object, or lambda expression that returns void and is
    // passed the value stored in each node (as a const reference, so no copy
    // is made; a visitor taking T by value still works).
    //
    template <typename Visitor>
    void preorder(Visitor visit) const {
        if (isEmpty())
            return;
        visit(root());
        left().preorder(visit);
        right().preorder(visit);
    }

    template <typename Visitor>
    void inorder(Visitor visit) const {
        if (isEmpty()) return;
        left().inorder(visit);
        visit(root());
        right().inorder(visit);
    }

    template <typename Visitor>
    void postorder(Visitor visit) const {
        if (isEmpty()) return;
        left().postorder(visit);
        right().postorder(visit);
        visit(root());
    }

private:
    std::shared_ptr<const Node> _root;
};


// Overload the << operator to print the tree in a human-readable format
// This is a simple implementation that prints the tree in a pre-order traversal.
// Note how the overload takes advantage of the preorder function to print the values in the tree
// and the ability to pass a callable object to the preorder function.
template<typename T, typename Compare>
std::ostream& operator<<(std::ostream& os, const Tree<T, Compare>& tree) {
if (tree.isEmpty()) {
os << "[]";
return os;
}
os << "[";
tree.preorder([&os](const T& val) { os << val << " "; });
os << "]";
return os;
}
