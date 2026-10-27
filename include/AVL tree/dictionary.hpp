//
// File: dictionary.hpp
// Purpose:
// An ordered associative container (key -> value) backed by an AVL tree of
// Pair<Key,Value>. Keys are unique and kept in sorted order.
//
#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>

#include "avltree.hpp"
#include "list.hpp"
#include "pair.hpp"

template <typename Key, typename Value>
class Dictionary {
public:
    using PairType = Pair<Key, Value>;

private:
    // The tree must order (and find) entries by KEY ONLY. Pair's defaulted <=>
    // compares `first` and then `second`, so using it directly would treat
    // {k, 1} and {k, 2} as different entries and lookups by key would fail.
    struct KeyLess {
        bool operator()(const PairType &a, const PairType &b) const {
            return a.first < b.first;
        }
    };

    AVLTree<PairType, KeyLess> tree_;
    std::size_t size_ = 0;

    // keys() / values() hand back references, so the lists must live
    // somewhere; they are rebuilt from the tree on every call.
    mutable List<Key> keys_;
    mutable List<Value> values_;

    // A probe pair for searching. Only `first` matters to KeyLess.
    static PairType probe(const Key &key) { return PairType{key, Value{}}; }

public:
    // ---- Rule of Five -------------------------------------------------------
    Dictionary() = default;

    Dictionary(const Dictionary &other)
        : tree_(other.tree_), size_(other.size_) {}

    Dictionary &operator=(const Dictionary &other) {
        if (this != &other) {
            tree_ = other.tree_;
            size_ = other.size_;
        }
        return *this;
    }

    Dictionary(Dictionary &&other) noexcept
        : tree_(std::move(other.tree_)), size_(std::exchange(other.size_, 0)) {}

    Dictionary &operator=(Dictionary &&other) noexcept {
        if (this != &other) {
            tree_ = std::move(other.tree_);
            size_ = std::exchange(other.size_, 0);
        }
        return *this;
    }

    ~Dictionary() = default;

    // ---- Access and modification -------------------------------------------

    // Returns the value for `key`; inserts {key, Value{}} first if absent.
    Value &operator[](const Key &key) {
        PairType target = probe(key);
        if (PairType *p = tree_.find(target)) return p->second;
        tree_.insert(target);
        ++size_;
        return tree_.find(target)->second;
    }

    // Returns the value for `key`; throws std::out_of_range if absent.
    Value &at(const Key &key) {
        if (PairType *p = tree_.find(probe(key))) return p->second;
        throw std::out_of_range("Dictionary::at: key not found");
    }

    const Value &at(const Key &key) const {
        if (const PairType *p = tree_.find(probe(key))) return p->second;
        throw std::out_of_range("Dictionary::at: key not found");
    }

    // Inserts the pair if its key is not present. Returns true if inserted,
    // false if the key already existed (the existing value is left unchanged).
    bool insert(const PairType &pair) {
        if (tree_.contains(pair)) return false;
        tree_.insert(pair);
        ++size_;
        return true;
    }

    bool insert(const Key &key, const Value &value) {
        return insert(PairType{key, value});
    }

    // Removes the entry for `key`. Returns true if something was removed.
    bool erase(const Key &key) {
        PairType target = probe(key);
        if (!tree_.contains(target)) return false;
        tree_.remove(target);
        --size_;
        return true;
    }

    void clear() noexcept {
        tree_.clear();
        size_ = 0;
    }

    // ---- Lookup -------------------------------------------------------------
    bool contains(const Key &key) const { return tree_.contains(probe(key)); }

    // Keys in ascending order.
    List<Key> &keys() const {
        keys_ = List<Key>{};
        tree_.inorder_traverse([this](const PairType &p) { keys_.push_back(p.first); });
        return keys_;
    }

    // Values in ascending order of their keys.
    List<Value> &values() const {
        values_ = List<Value>{};
        tree_.inorder_traverse([this](const PairType &p) { values_.push_back(p.second); });
        return values_;
    }

    // ---- Capacity and size --------------------------------------------------
    bool isEmpty() const noexcept { return size_ == 0; }
    std::size_t size() const noexcept { return size_; }

    // ---- Convenience --------------------------------------------------------
    template <typename Func>
    void forEach(Func &&f) const { tree_.inorder_traverse(std::forward<Func>(f)); }

    void display() const { tree_.display(); }
};
