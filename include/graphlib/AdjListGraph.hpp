//
//  File:   AdjListGraph.hpp
//  Author: Your Glorious Instructor
//  Purpose:
//  Implementation of the adjacency list implementation of the graph ADT
//

#pragma once

#include "Graph.hpp"
#include <map>
#include <vector>
#include <list>
#include <algorithm>

template <class N>
class AdjListGraph : public Graph<N> {
private:
    using Edge = typename Graph<N>::Edge;
    using EdgeList = std::list<Edge>;
    std::map<N, EdgeList> vertexMap;

public:
    // Default constructor, create empty
    AdjListGraph() = default;

    // Get the vertex map from the Graph we're copying
    AdjListGraph(const AdjListGraph& other) : Graph<N>() {
        vertexMap = other.vertexMap;
    }

    AdjListGraph& operator= (const AdjListGraph &source) {
        vertexMap = source.vertexMap;
        return *this;
    }

    // Add the nodes in the list to graph
    AdjListGraph(const std::vector<N>& newNodes, const std::vector<Edge>& newEdgeVec) {
        for (const N& node : newNodes) addNode(node);
        for (const Edge& e : newEdgeVec) addEdge(e.first, e.second);
    }


    // Clean up behind ourselves, we're not doing anything with
    // dynamic memory so can use system provided destructor.
    ~AdjListGraph() = default;

    bool adjacent(const N& x, const N& y) const override {
        auto it = vertexMap.find(x);
        if (it == vertexMap.end()) return false;
        const Edge search{x,y};
        const auto &edges = it->second;
        auto pos = std::find(edges.begin(), edges.end(), search);
        return pos != edges.end();
    }

    std::vector<N> neighbors(const N& x) const override {
        std::vector<N> nodes;
        auto it = vertexMap.find(x);
        if (it == vertexMap.end()) return nodes;
        for (const auto &e : it->second) nodes.push_back(e.second);
        return nodes;
    }

    void addNode(const N& node) override {
        if (vertexMap.find(node) == vertexMap.end()) vertexMap[node] = EdgeList{};
    }

    void addEdge(const N& x, const N& y) override {
        addNode(x);
        addNode(y);
        Edge forward{x,y};
        auto &lst = vertexMap[x];
        if (std::find(lst.begin(), lst.end(), forward) == lst.end()) lst.push_back(forward);
    }

    void deleteEdge(const N& x, const N& y) override {
        auto it = vertexMap.find(x);
        if (it == vertexMap.end()) return;
        it->second.remove(Edge{x,y});
    }

    bool hasNode(const N& x) const override {
        return vertexMap.find(x) != vertexMap.end();
    }

    std::vector<N> getNodes() const override {
        std::vector<N> keys;
        keys.reserve(vertexMap.size());
        for (const auto &p : vertexMap) keys.push_back(p.first);
        return keys;
    }

    std::vector<Edge> getEdges() const override {
        std::vector<Edge> edges;
        for (const auto &p : vertexMap) {
            for (const auto &e : p.second) edges.push_back(e);
        }
        return edges;
    }

    void removeNode(const N& x) override {
        vertexMap.erase(x);
        for (auto &p : vertexMap) {
            p.second.remove_if([&](const Edge &e){ return e.second == x; });
        }
    }
};
