//
//  File:   Graph.hpp
//  Author: Your Glorious Instructor
//  Purpose:
//  Abstract base class (pure virtual class) for Graphs
//
#pragma once
#include <vector>
#include <utility>
#include <functional>
#include <map>

template <class N>
class Graph {
public:
    using Edge = std::pair<N, N>;

    Graph() = default;
    virtual ~Graph() = default;

    // Core operations to be implemented by derived classes
    virtual bool adjacent(const N& x, const N& y) const = 0;
    virtual std::vector<N> neighbors(const N& x) const = 0;
    virtual void addNode(const N& x) = 0;
    virtual void addEdge(const N& x, const N& y) = 0;
    virtual void deleteEdge(const N& x, const N& y) = 0;

    // Convenience helpers for traversal and introspection
    virtual bool hasNode(const N& x) const = 0;
    virtual std::vector<N> getNodes() const = 0;
    virtual std::vector<Edge> getEdges() const = 0;
    virtual void removeNode(const N& x) = 0;

    // Traversals implemented once in the base class using the primitives above.
    // visit is optional and receives nodes by const reference. The returned map
    // maps each discovered node to its parent (start node maps to itself).
    virtual std::map<N, N> dfs(const N& startNode, std::function<void(const N&)> visit = {}) const {
        std::map<N, N> parent;
        if (!hasNode(startNode)) return parent;

        std::map<N, bool> visited;
        for (const auto &n : getNodes()) visited[n] = false;

        std::vector<N> stk;
        stk.push_back(startNode);
        parent[startNode] = startNode;

        while (!stk.empty()) {
            N cur = stk.back();
            stk.pop_back();
            if (visited[cur]) continue;
            if (visit) visit(cur);
            visited[cur] = true;
            for (const auto &nbr : neighbors(cur)) {
                if (!visited[nbr]) {
                    parent[nbr] = cur;
                    stk.push_back(nbr);
                }
            }
        }
        return parent;
    }

    virtual std::map<N, N> bfs(const N& startNode, std::function<void(const N&)> visit = {}) const {
        std::map<N, N> parent;
        if (!hasNode(startNode)) return parent;

        std::map<N, bool> visited;
        for (const auto &n : getNodes()) visited[n] = false;

        std::vector<N> q;
        size_t head = 0;
        q.push_back(startNode);
        visited[startNode] = true;
        parent[startNode] = startNode;

        while (head < q.size()) {
            N cur = q[head++];
            if (visit) visit(cur);
            for (const auto &nbr : neighbors(cur)) {
                if (!visited[nbr]) {
                    visited[nbr] = true;
                    parent[nbr] = cur;
                    q.push_back(nbr);
                }
            }
        }
        return parent;
    }
};

