//
//  File:   WeightedAdjMatrixGraph.hpp
//  Author: Your Glorious Instructor (fixed and updated to the current Graph ADT)
//  Purpose:
//  Implementation of the weighted adjacency matrix representation of the graph ADT.
//
//  Changes from the original sample:
//   * Matrix grows dynamically (no fixed maxSize = 10).
//   * Edge presence is stored separately from the weight, so an edge with
//     weight 0 (what addEdge() creates) is still an edge.
//   * Signatures match Graph<N> (const refs, const methods) so the class is
//     concrete and the base-class dfs()/bfs() are inherited instead of the
//     broken copies that never returned a value.
//   * No leaked `new std::vector` in neighbors().
//
#pragma once

#include "Graph.hpp"

#include <list>
#include <tuple>
#include <vector>

template <class N>
class WeightedAdjMatrixGraph : public Graph<N> {
public:
    using WeightedEdge = std::tuple<N, N, int>;
    using EdgeList = std::list<WeightedEdge>;

private:
    std::vector<N> nodes;
    std::vector<std::vector<char>> present;  // present[i][j] != 0 -> edge i->j
    std::vector<std::vector<int>> weights;   // weight of edge i->j

    int findIndex(const N& x) const {
        for (int i = 0; i < static_cast<int>(nodes.size()); ++i) {
            if (nodes[i] == x) return i;
        }
        return -1;
    }

public:
    WeightedAdjMatrixGraph() = default;

    WeightedAdjMatrixGraph(const std::vector<N>& newNodes,
                           const std::vector<WeightedEdge>& newEdges) {
        for (const N& n : newNodes) addNode(n);
        for (const WeightedEdge& e : newEdges) {
            addEdgeWithWeight(std::get<0>(e), std::get<1>(e), std::get<2>(e));
        }
    }

    bool adjacent(const N& x, const N& y) const override {
        int ix = findIndex(x);
        int iy = findIndex(y);
        if (ix < 0 || iy < 0) return false;
        return present[ix][iy] != 0;
    }

    std::vector<N> neighbors(const N& x) const override {
        std::vector<N> res;
        int ix = findIndex(x);
        if (ix < 0) return res;
        for (int j = 0; j < static_cast<int>(nodes.size()); ++j) {
            if (present[ix][j]) res.push_back(nodes[j]);
        }
        return res;
    }

    void addNode(const N& node) override {
        if (findIndex(node) != -1) return;
        nodes.push_back(node);
        for (auto& row : present) row.push_back(0);
        for (auto& row : weights) row.push_back(0);
        present.emplace_back(nodes.size(), 0);
        weights.emplace_back(nodes.size(), 0);
    }

    void addEdge(const N& x, const N& y) override { addEdgeWithWeight(x, y, 0); }

    void deleteEdge(const N& x, const N& y) override {
        int ix = findIndex(x);
        int iy = findIndex(y);
        if (ix < 0 || iy < 0) return;
        present[ix][iy] = 0;
        weights[ix][iy] = 0;
    }

    bool hasNode(const N& x) const override { return findIndex(x) != -1; }

    std::vector<N> getNodes() const override { return nodes; }

    std::vector<typename Graph<N>::Edge> getEdges() const override {
        std::vector<typename Graph<N>::Edge> edges;
        for (int i = 0; i < static_cast<int>(nodes.size()); ++i) {
            for (int j = 0; j < static_cast<int>(nodes.size()); ++j) {
                if (present[i][j]) edges.emplace_back(nodes[i], nodes[j]);
            }
        }
        return edges;
    }

    void removeNode(const N& x) override {
        int idx = findIndex(x);
        if (idx < 0) return;
        nodes.erase(nodes.begin() + idx);
        present.erase(present.begin() + idx);
        weights.erase(weights.begin() + idx);
        for (auto& row : present) row.erase(row.begin() + idx);
        for (auto& row : weights) row.erase(row.begin() + idx);
    }

    // ---- Weighted extras (same API as WeightedAdjListGraph) ----

    void addEdgeWithWeight(const N& x, const N& y, int weight = 0) {
        addNode(x);
        addNode(y);
        int ix = findIndex(x);
        int iy = findIndex(y);
        present[ix][iy] = 1;
        weights[ix][iy] = weight;
    }

    // Removes x->y only if it currently has the given weight.
    void deleteEdgeWithWeight(const N& x, const N& y, int weight) {
        int ix = findIndex(x);
        int iy = findIndex(y);
        if (ix < 0 || iy < 0) return;
        if (present[ix][iy] && weights[ix][iy] == weight) {
            present[ix][iy] = 0;
            weights[ix][iy] = 0;
        }
    }

    // Returns 0 if the edge doesn't exist (matches WeightedAdjListGraph).
    int getEdgeWeight(const N& x, const N& y) const {
        int ix = findIndex(x);
        int iy = findIndex(y);
        if (ix < 0 || iy < 0 || !present[ix][iy]) return 0;
        return weights[ix][iy];
    }

    EdgeList getEdges(const N& node) const {
        EdgeList edges;
        int ix = findIndex(node);
        if (ix < 0) return edges;
        for (int j = 0; j < static_cast<int>(nodes.size()); ++j) {
            if (present[ix][j]) edges.emplace_back(node, nodes[j], weights[ix][j]);
        }
        return edges;
    }
};
