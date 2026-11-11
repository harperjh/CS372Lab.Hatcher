//
//  File:   WeightedAdjListGraph.hpp
//  Author: Your Glorious Instructor
//  Purpose:
//  Implementation of the weighted adjacency list representation of the graph ADT
//

#pragma once

#include "Graph.hpp"

#include <algorithm>
#include <list>
#include <map>
#include <tuple>
#include <vector>

template <class N>
class WeightedAdjListGraph : public Graph<N> {
private:
    using WeightedEdge = std::tuple<N, N, int>;
    using EdgeList = std::list<WeightedEdge>;
    std::map<N, EdgeList> vertexMap;

public:
    WeightedAdjListGraph() = default;

    WeightedAdjListGraph(const WeightedAdjListGraph&) = default;
    WeightedAdjListGraph& operator=(const WeightedAdjListGraph&) = default;
    ~WeightedAdjListGraph() override = default;

    WeightedAdjListGraph(const std::vector<N>& newNodes,
                         const std::vector<WeightedEdge>& newEdges) {
        for (const N& node : newNodes) {
            addNode(node);
        }
        for (const WeightedEdge& edge : newEdges) {
            addEdgeWithWeight(std::get<0>(edge), std::get<1>(edge),
                              std::get<2>(edge));
        }
    }

    bool adjacent(const N& x, const N& y) const override {
        const auto node = vertexMap.find(x);
        if (node == vertexMap.end()) {
            return false;
        }
        return std::find_if(node->second.begin(), node->second.end(),
                            [&](const WeightedEdge& edge) {
                                return std::get<1>(edge) == y;
                            }) != node->second.end();
    }

    std::vector<N> neighbors(const N& x) const override {
        std::vector<N> result;
        const auto node = vertexMap.find(x);
        if (node == vertexMap.end()) {
            return result;
        }
        result.reserve(node->second.size());
        for (const WeightedEdge& edge : node->second) {
            result.push_back(std::get<1>(edge));
        }
        return result;
    }

    void addNode(const N& node) override {
        vertexMap.try_emplace(node);
    }

    void addEdge(const N& x, const N& y) override {
        addEdgeWithWeight(x, y);
    }

    void deleteEdge(const N& x, const N& y) override {
        const auto node = vertexMap.find(x);
        if (node != vertexMap.end()) {
            node->second.remove_if([&](const WeightedEdge& edge) {
                return std::get<1>(edge) == y;
            });
        }
    }

    bool hasNode(const N& node) const override {
        return vertexMap.find(node) != vertexMap.end();
    }

    std::vector<N> getNodes() const override {
        std::vector<N> nodes;
        nodes.reserve(vertexMap.size());
        for (const auto& entry : vertexMap) {
            nodes.push_back(entry.first);
        }
        return nodes;
    }

    std::vector<typename Graph<N>::Edge> getEdges() const override {
        std::vector<typename Graph<N>::Edge> edges;
        for (const auto& entry : vertexMap) {
            for (const WeightedEdge& edge : entry.second) {
                edges.emplace_back(std::get<0>(edge), std::get<1>(edge));
            }
        }
        return edges;
    }

    void removeNode(const N& node) override {
        vertexMap.erase(node);
        for (auto& entry : vertexMap) {
            entry.second.remove_if([&](const WeightedEdge& edge) {
                return std::get<1>(edge) == node;
            });
        }
    }

    void addEdgeWithWeight(const N& x, const N& y, int weight = 0) {
        addNode(x);
        addNode(y);
        EdgeList& edges = vertexMap[x];
        const auto existing = std::find_if(
            edges.begin(), edges.end(), [&](const WeightedEdge& edge) {
                return std::get<1>(edge) == y;
            });
        if (existing == edges.end()) {
            edges.emplace_back(x, y, weight);
        } else {
            std::get<2>(*existing) = weight;
        }
    }

    void deleteEdgeWithWeight(const N& x, const N& y, int weight) {
        const auto node = vertexMap.find(x);
        if (node != vertexMap.end()) {
            node->second.remove_if([&](const WeightedEdge& edge) {
                return std::get<1>(edge) == y && std::get<2>(edge) == weight;
            });
        }
    }

    int getEdgeWeight(const N& x, const N& y) const {
        const auto node = vertexMap.find(x);
        if (node == vertexMap.end()) {
            return 0;
        }
        const auto edge = std::find_if(
            node->second.begin(), node->second.end(),
            [&](const WeightedEdge& candidate) {
                return std::get<1>(candidate) == y;
            });
        return edge == node->second.end() ? 0 : std::get<2>(*edge);
    }

    std::list<WeightedEdge> getEdges(const N& node) const {
        const auto entry = vertexMap.find(node);
        return entry == vertexMap.end() ? std::list<WeightedEdge>{}
                                         : entry->second;
    }
};
