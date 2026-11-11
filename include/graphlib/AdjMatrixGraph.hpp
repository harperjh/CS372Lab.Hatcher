//
//  File:   AdjMatrixGraph.hpp
//  Author: Your Glorious Instructor
//  Purpose:
//  Implementation of the adjacency matrix implementation of the graph ADT
//
#pragma once
#include "Graph.hpp"
#include <vector>
#include <map>
#include <algorithm>

template <class N>
class AdjMatrixGraph : public Graph<N> {
private:
    using Edge = typename Graph<N>::Edge;
    std::vector<N> nodes;
    std::vector<std::vector<char>> matrix; // dynamic adjacency matrix

    int findIndex(const N& x) const {
        for (int i = 0; i < static_cast<int>(nodes.size()); ++i) {
            if (nodes[i] == x) return i;
        }
        return -1;
    }

public:
    AdjMatrixGraph() = default;

    AdjMatrixGraph(const std::vector<N>& newNodes, const std::vector<Edge>& newEdges) {
        for (const auto &n : newNodes) addNode(n);
        for (const auto &e : newEdges) addEdge(e.first, e.second);
    }

    bool adjacent(const N& x, const N& y) const override {
        int ix = findIndex(x);
        int iy = findIndex(y);
        if (ix < 0 || iy < 0) return false;
        return matrix[ix][iy] != 0;
    }

    std::vector<N> neighbors(const N& x) const override {
        std::vector<N> res;
        int ix = findIndex(x);
        if (ix < 0) return res;
        for (int j = 0; j < static_cast<int>(nodes.size()); ++j) {
            if (matrix[ix][j]) res.push_back(nodes[j]);
        }
        return res;
    }

    void addNode(const N& node) override {
        if (findIndex(node) != -1) return;
        nodes.push_back(node);
        for (auto &row : matrix) row.push_back(0);
        matrix.emplace_back(nodes.size(), 0);
    }

    void addEdge(const N& x, const N& y) override {
        addNode(x);
        addNode(y);
        int ix = findIndex(x);
        int iy = findIndex(y);
        if (ix >= 0 && iy >= 0) matrix[ix][iy] = 1;
    }

    void deleteEdge(const N& x, const N& y) override {
        int ix = findIndex(x);
        int iy = findIndex(y);
        if (ix < 0 || iy < 0) return;
        matrix[ix][iy] = 0;
    }

    bool hasNode(const N& x) const override {
        return findIndex(x) != -1;
    }

    std::vector<N> getNodes() const override {
        return nodes;
    }

    std::vector<Edge> getEdges() const override {
        std::vector<Edge> edges;
        for (int i = 0; i < static_cast<int>(nodes.size()); ++i) {
            for (int j = 0; j < static_cast<int>(nodes.size()); ++j) {
                if (matrix[i][j]) edges.emplace_back(nodes[i], nodes[j]);
            }
        }
        return edges;
    }

    void removeNode(const N& x) override {
        int idx = findIndex(x);
        if (idx < 0) return;
        nodes.erase(nodes.begin() + idx);
        matrix.erase(matrix.begin() + idx);
        for (auto &row : matrix) row.erase(row.begin() + idx);
    }
};
