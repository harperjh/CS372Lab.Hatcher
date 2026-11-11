//
//  File:   kevin_bacon.cpp
//  Purpose:
//  Demo: build the 20-person social graph and print my Kevin Bacon path and number
//  using both the adjacency list and adjacency matrix representations.
//
//  Build & run (from the project root):
//      g++ -std=c++17 -Iinclude kevin_bacon.cpp -o kevin_bacon
//      ./kevin_bacon
//
#include "AdjListGraph.hpp"
#include "AdjMatrixGraph.hpp"
#include "GraphAlgorithms.hpp"
#include "SocialGraph.hpp"

#include <iostream>
#include <list>
#include <string>

static void report(const std::string& label, const Graph<std::string>& g,
                   const std::string& who) {
    std::list<std::string> path;
    std::cout << "[" << label << "] " << who << ": ";
    if (connectedToKevinBacon(g, path, who)) {
        bool first = true;
        for (const auto& p : path) {
            std::cout << (first ? "" : " -> ") << p;
            first = false;
        }
        std::cout << "\n    Kevin Bacon number = " << kevinBaconNumber(path) << "\n";
    } else {
        std::cout << "not connected to Kevin Bacon\n";
    }
}

int main() {
    AdjListGraph<std::string> listGraph;
    AdjMatrixGraph<std::string> matrixGraph;
    buildSocialGraph(listGraph);
    buildSocialGraph(matrixGraph);

    report("adjacency list  ", listGraph, ME);
    report("adjacency matrix", matrixGraph, ME);
    report("adjacency list  ", listGraph, "Zoe Adams");
    return 0;
}
