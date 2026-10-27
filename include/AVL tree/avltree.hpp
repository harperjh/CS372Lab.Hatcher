//
// File: avltree.hpp
// Author: Your Glorious Instructor
// Purpose: 
// Provide a balanced AVL Tree (C++20, unique_ptr ownership, deep-copy, templated visitor)
//
#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <functional>

template <typename T, typename Compare = std::less<T>>
class AVLTree {
private:
    struct Node {
        T key;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        int height = 1;
        explicit Node(const T& k) : key(k) {}
        explicit Node(T&& k) : key(std::move(k)) {}
    };

    std::unique_ptr<Node> root_;
    Compare comp_{};

    static int heightOf(const Node* n) noexcept { return n ? n->height : 0; }
    static int balanceOf(const Node* n) noexcept { return n ? heightOf(n->left.get()) - heightOf(n->right.get()) : 0; }

    std::unique_ptr<Node> rotateRight(std::unique_ptr<Node> y) {
        auto x = std::move(y->left);
        y->left = std::move(x->right);
        x->right = std::move(y);
        x->right->height = 1 + std::max(heightOf(x->right->left.get()), heightOf(x->right->right.get()));
        x->height = 1 + std::max(heightOf(x->left.get()), heightOf(x->right.get()));
        return x;
    }

    std::unique_ptr<Node> rotateLeft(std::unique_ptr<Node> x) {
        auto y = std::move(x->right);
        x->right = std::move(y->left);
        y->left = std::move(x);
        y->left->height = 1 + std::max(heightOf(y->left->left.get()), heightOf(y->left->right.get()));
        y->height = 1 + std::max(heightOf(y->left.get()), heightOf(y->right.get()));
        return y;
    }

    std::unique_ptr<Node> insertNode(std::unique_ptr<Node> node, const T& key) {
        if (!node) return std::make_unique<Node>(key);

        if (comp_(key, node->key))
            node->left = insertNode(std::move(node->left), key);
        else if (comp_(node->key, key))
            node->right = insertNode(std::move(node->right), key);
        else
            return node; // duplicate

        node->height = 1 + std::max(heightOf(node->left.get()), heightOf(node->right.get()));
        int balance = balanceOf(node.get());

        // LL
        if (balance > 1 && comp_(key, node->left->key))
            return rotateRight(std::move(node));
        // RR
        if (balance < -1 && comp_(node->right->key, key))
            return rotateLeft(std::move(node));
        // LR
        if (balance > 1 && comp_(node->left->key, key)) {
            node->left = rotateLeft(std::move(node->left));
            return rotateRight(std::move(node));
        }
        // RL
        if (balance < -1 && comp_(key, node->right->key)) {
            node->right = rotateRight(std::move(node->right));
            return rotateLeft(std::move(node));
        }

        return node;
    }

    Node* minNode(Node* n) const noexcept {
        while (n && n->left) n = n->left.get();
        return n;
    }

    std::unique_ptr<Node> deleteNode(std::unique_ptr<Node> node, const T& key) {
        if (!node) return node;

        if (comp_(key, node->key))
            node->left = deleteNode(std::move(node->left), key);
        else if (comp_(node->key, key))
            node->right = deleteNode(std::move(node->right), key);
        else {
            if (!node->left || !node->right) {
                return node->left ? std::move(node->left) : std::move(node->right);
            } else {
                Node* succ = minNode(node->right.get());
                node->key = succ->key;
                // pass node->key (now equal to the successor's key): it stays alive,
                // whereas succ->key would dangle once the successor is freed
                node->right = deleteNode(std::move(node->right), node->key);
            }
        }

        if (!node) return node;

        node->height = 1 + std::max(heightOf(node->left.get()), heightOf(node->right.get()));
        int balance = balanceOf(node.get());

        // LL
        if (balance > 1 && balanceOf(node->left.get()) >= 0)
            return rotateRight(std::move(node));
        // LR
        if (balance > 1 && balanceOf(node->left.get()) < 0) {
            node->left = rotateLeft(std::move(node->left));
            return rotateRight(std::move(node));
        }
        // RR
        if (balance < -1 && balanceOf(node->right.get()) <= 0)
            return rotateLeft(std::move(node));
        // RL
        if (balance < -1 && balanceOf(node->right.get()) > 0) {
            node->right = rotateRight(std::move(node->right));
            return rotateLeft(std::move(node));
        }

        return node;
    }

    std::unique_ptr<Node> clone(const std::unique_ptr<Node>& n) const {
        if (!n) return nullptr;
        auto copy = std::make_unique<Node>(n->key);
        copy->height = n->height;
        copy->left = clone(n->left);
        copy->right = clone(n->right);
        return copy;
    }

    Node* findNode(const T& key) const noexcept {
        Node* n = root_.get();
        while (n) {
            if (comp_(key, n->key))      n = n->left.get();
            else if (comp_(n->key, key)) n = n->right.get();
            else                         return n;
        }
        return nullptr;
    }

    template <typename Func>
    void inorder_impl(const Node* n, Func&& f) const {
        if (!n) return;
        inorder_impl(n->left.get(), std::forward<Func>(f));
        f(n->key);
        inorder_impl(n->right.get(), std::forward<Func>(f));
    }

public:
    AVLTree() = default;
    AVLTree(const AVLTree& other) : root_(clone(other.root_)), comp_(other.comp_) {}
    AVLTree& operator=(const AVLTree& other) {
        if (this != &other) {
            root_ = clone(other.root_);
            comp_ = other.comp_;
        }
        return *this;
    }
    AVLTree(AVLTree&&) noexcept = default;
    AVLTree& operator=(AVLTree&&) noexcept = default;
    ~AVLTree() = default;

    void insert(const T& key) { root_ = insertNode(std::move(root_), key); }
    void remove(const T& key) { root_ = deleteNode(std::move(root_), key); }

    // Lookup: returns a pointer to the stored element that compares equivalent
    // to `key`, or nullptr. The non-const version lets callers modify parts of
    // the element that do NOT participate in the ordering.
    T* find(const T& key) noexcept {
        Node* n = findNode(key);
        return n ? &n->key : nullptr;
    }
    const T* find(const T& key) const noexcept {
        const Node* n = findNode(key);
        return n ? &n->key : nullptr;
    }
    bool contains(const T& key) const noexcept { return findNode(key) != nullptr; }

    void clear() noexcept { root_.reset(); }
    bool empty() const noexcept { return !root_; }

    template <typename Func>
    void inorder_traverse(Func&& f) const { inorder_impl(root_.get(), std::forward<Func>(f)); }

    // convenience
    void display() const {
        inorder_traverse([](const T& k){ std::cout << k << ' '; });
        std::cout << '\n';
    }
};