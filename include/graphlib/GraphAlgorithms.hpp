//
//  File:   GraphAlgorithms.hpp
//  Purpose:
//  Algorithms written only against the Graph<N> interface, so they work with
//  any representation (AdjListGraph, AdjMatrixGraph, and the weighted ones).
//
#pragma once

#include "Graph.hpp"

#include <list>
#include <set>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// isSimpleCycle
//
// Returns true if `path` is a simple cycle in `g`:
//   1. It begins and ends at the same node.
//   2. Each consecutive pair (path[i], path[i+1]) is an edge in g, i.e. each
//      node is a neighbor of the one before it ("Won't you be my neighbor?").
//      Edges are directed, following the graph classes, so for an undirected
//      graph store each edge in both directions.
//   3. No node appears twice, except that the first node appears again at the end.
//
// The cycle must have at least 3 distinct nodes (e.g. {1,2,3,1}). That rules
// out {1,2,1}, which in an undirected graph just goes along one edge and back
// again, and {1,1}, a single self-loop.
// ---------------------------------------------------------------------------
template <class N>
bool isSimpleCycle(const Graph<N>& g, const std::vector<N>& path) {
    if (path.size() < 4) return false;            // need >= 3 distinct nodes
    if (path.front() != path.back()) return false; // must close the loop

    std::set<N> seen;
    for (std::size_t i = 0; i + 1 < path.size(); ++i) {
        const N& cur = path[i];
        const N& next = path[i + 1];
        if (!g.hasNode(cur)) return false;              // node must exist
        if (!seen.insert(cur).second) return false;     // repeated node -> not simple
        if (!g.adjacent(cur, next)) return false;       // next must be cur's neighbor
    }
    return true;
}

// ---------------------------------------------------------------------------
// findShortestPath
//
// Breadth-first search from `from`. If `to` is reachable, fills `path` with
// the nodes from `from` to `to` (both included) along a shortest path and
// returns true. Otherwise clears `path` and returns false.
// ---------------------------------------------------------------------------
template <class N>
bool findShortestPath(const Graph<N>& g, const N& from, const N& to, std::list<N>& path) {
    path.clear();
    if (!g.hasNode(from) || !g.hasNode(to)) return false;

    const std::map<N, N> parent = g.bfs(from);  // child -> parent, from -> from
    if (parent.find(to) == parent.end()) return false;

    // Walk the parent links back from `to` to `from`.
    N cur = to;
    path.push_front(cur);
    while (!(cur == from)) {
        cur = parent.at(cur);
        path.push_front(cur);
    }
    return true;
}

// ---------------------------------------------------------------------------
// Seven Degrees of Kevin Bacon
// ---------------------------------------------------------------------------
inline const std::string KEVIN_BACON = "Kevin Bacon";
inline const std::string ME = "Ethan Li";

// Returns true if `me` is connected to Kevin Bacon in the social graph.
// `path` receives the people from `me` to Kevin Bacon along a shortest path,
// or is left empty if there is no connection.
inline bool connectedToKevinBacon(const Graph<std::string>& socialGraph,
                                  std::list<std::string>& path,
                                  const std::string& me = ME) {
    return findShortestPath(socialGraph, me, KEVIN_BACON, path);
}

// Kevin Bacon number = number of links (edges) in the path.
// Kevin Bacon himself is 0; returns -1 when there is no path.
inline int kevinBaconNumber(const std::list<std::string>& path) {
    return path.empty() ? -1 : static_cast<int>(path.size()) - 1;
}
