//
//  File:   test_graphs.cpp
//  Purpose:
//  Test script for the graph library. Every test is a template that is run
//  against all four representations:
//      AdjListGraph, AdjMatrixGraph, WeightedAdjListGraph, WeightedAdjMatrixGraph
//
//  Build & run (from the project root):
//      g++ -std=c++17 -Iinclude tests/test_graphs.cpp -o test_graphs
//      ./test_graphs
//
#include "AdjListGraph.hpp"
#include "AdjMatrixGraph.hpp"
#include "GraphAlgorithms.hpp"
#include "SocialGraph.hpp"
#include "WeightedAdjListGraph.hpp"
#include "WeightedAdjMatrixGraph.hpp"

#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Tiny test harness
// ---------------------------------------------------------------------------
static int g_passed = 0;
static int g_failed = 0;
static std::string g_suite;

#define CHECK(cond)                                                              \
    do {                                                                         \
        if (cond) {                                                              \
            ++g_passed;                                                          \
        } else {                                                                 \
            ++g_failed;                                                          \
            std::cerr << "  FAIL [" << g_suite << "] " << __FILE__ << ":"        \
                      << __LINE__ << "  " #cond << "\n";                         \
        }                                                                        \
    } while (0)

template <class N>
static bool contains(const std::vector<N>& v, const N& x) {
    return std::find(v.begin(), v.end(), x) != v.end();
}

// True if every consecutive pair in `path` is an edge in `g`.
template <class N>
static bool isValidPath(const Graph<N>& g, const std::list<N>& path) {
    if (path.empty()) return false;
    auto it = path.begin();
    auto next = std::next(it);
    for (; next != path.end(); ++it, ++next) {
        if (!g.adjacent(*it, *next)) return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Part 1: basic Graph ADT behaviour, same for every representation
// ---------------------------------------------------------------------------
template <template <class> class G>
void testBasicADT() {
    G<int> g;
    CHECK(g.getNodes().empty());
    CHECK(!g.hasNode(1));

    g.addNode(1);
    g.addNode(2);
    g.addNode(2);  // duplicate should be ignored
    CHECK(g.getNodes().size() == 2);
    CHECK(g.hasNode(1) && g.hasNode(2));

    g.addEdge(1, 2);
    CHECK(g.adjacent(1, 2));
    CHECK(!g.adjacent(2, 1));  // directed
    CHECK(g.neighbors(1) == std::vector<int>{2});
    CHECK(g.neighbors(2).empty());

    g.addEdge(2, 3);  // addEdge creates missing nodes
    CHECK(g.hasNode(3));
    CHECK(g.adjacent(2, 3));
    CHECK(g.getEdges().size() == 2);

    g.addEdge(1, 2);  // duplicate edge should be ignored
    CHECK(g.getEdges().size() == 2);

    g.deleteEdge(1, 2);
    CHECK(!g.adjacent(1, 2));
    CHECK(g.getEdges().size() == 1);

    g.deleteEdge(42, 43);  // deleting from missing nodes is a no-op
    CHECK(g.getEdges().size() == 1);

    // neighbors / adjacent on missing nodes
    CHECK(g.neighbors(99).empty());
    CHECK(!g.adjacent(99, 1));

    // removeNode removes the node and all edges into and out of it
    g.addEdge(1, 3);
    g.addEdge(3, 1);
    g.removeNode(3);
    CHECK(!g.hasNode(3));
    CHECK(g.neighbors(1).empty());
    CHECK(g.neighbors(2).empty());
    CHECK(g.getEdges().empty());
    CHECK(g.getNodes().size() == 2);
}

template <template <class> class G>
void testTraversals() {
    // 1 -> 2 -> 4
    // 1 -> 3 -> 4,  5 is unreachable
    G<int> g;
    for (int n : {1, 2, 3, 4, 5}) g.addNode(n);
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 4);
    g.addEdge(3, 4);

    std::vector<int> bfsOrder;
    auto bfsParent = g.bfs(1, [&](const int& n) { bfsOrder.push_back(n); });
    CHECK(bfsOrder.size() == 4);
    CHECK(bfsOrder.front() == 1);
    CHECK(bfsOrder.back() == 4);  // 4 is the only node at distance 2
    CHECK(!contains(bfsOrder, 5));
    CHECK(bfsParent.at(1) == 1);
    CHECK(bfsParent.at(2) == 1 && bfsParent.at(3) == 1);
    CHECK(bfsParent.count(5) == 0);

    std::vector<int> dfsOrder;
    auto dfsParent = g.dfs(1, [&](const int& n) { dfsOrder.push_back(n); });
    CHECK(dfsOrder.size() == 4);
    CHECK(dfsOrder.front() == 1);
    CHECK(dfsParent.at(1) == 1);
    CHECK(dfsParent.count(5) == 0);

    CHECK(g.bfs(99).empty());  // missing start node
    CHECK(g.dfs(99).empty());
}

// Weighted-only features (template instantiated just for the weighted classes)
template <template <class> class G>
void testWeighted() {
    G<std::string> g;
    g.addEdgeWithWeight("A", "B", 7);
    g.addEdgeWithWeight("A", "C", 3);
    g.addEdge("B", "C");  // weight 0, still an edge
    CHECK(g.adjacent("A", "B"));
    CHECK(g.adjacent("B", "C"));
    CHECK(g.getEdgeWeight("A", "B") == 7);
    CHECK(g.getEdgeWeight("A", "C") == 3);
    CHECK(g.getEdgeWeight("B", "C") == 0);
    CHECK(g.getEdgeWeight("C", "A") == 0);  // missing edge
    CHECK(g.getEdges("A").size() == 2);

    g.addEdgeWithWeight("A", "B", 9);  // update weight, no duplicate edge
    CHECK(g.getEdgeWeight("A", "B") == 9);
    CHECK(g.getEdges("A").size() == 2);

    g.deleteEdgeWithWeight("A", "B", 1);  // wrong weight -> not removed
    CHECK(g.adjacent("A", "B"));
    g.deleteEdgeWithWeight("A", "B", 9);
    CHECK(!g.adjacent("A", "B"));
}

// ---------------------------------------------------------------------------
// Part 2: isSimpleCycle
// ---------------------------------------------------------------------------
template <template <class> class G>
void testIsSimpleCycle() {
    // Directed test graph:
    //   triangle  1 -> 2 -> 3 -> 1
    //   triangle  3 -> 4 -> 5 -> 3   (shares node 3 with the first one)
    //   back edge 2 -> 1             (makes {1,2,1} a valid closed path)
    //   square    6 -> 7 -> 8 -> 9 -> 6
    //   self loop 10 -> 10
    G<int> g;
    const std::vector<std::pair<int, int>> edges = {
        {1, 2}, {2, 3}, {3, 1}, {3, 4}, {4, 5}, {5, 3}, {2, 1},
        {6, 7}, {7, 8}, {8, 9}, {9, 6}, {10, 10}};
    for (auto [a, b] : edges) g.addEdge(a, b);

    // --- valid simple cycles
    CHECK(isSimpleCycle<int>(g, {1, 2, 3, 1}));
    CHECK(isSimpleCycle<int>(g, {2, 3, 1, 2}));     // same cycle, different start
    CHECK(isSimpleCycle<int>(g, {3, 4, 5, 3}));
    CHECK(isSimpleCycle<int>(g, {6, 7, 8, 9, 6}));

    // --- not closed
    CHECK(!isSimpleCycle<int>(g, {1, 2, 3}));
    CHECK(!isSimpleCycle<int>(g, {6, 7, 8, 9}));

    // --- edge missing / wrong direction
    CHECK(!isSimpleCycle<int>(g, {1, 3, 2, 1}));    // 1->3 doesn't exist
    CHECK(!isSimpleCycle<int>(g, {6, 7, 8, 6}));    // 8->6 doesn't exist
    CHECK(!isSimpleCycle<int>(g, {6, 9, 8, 7, 6})); // reverse of directed square

    // --- repeats a node (not simple)
    CHECK(!isSimpleCycle<int>(g, {1, 2, 3, 4, 5, 3, 1}));  // figure eight through 3
    CHECK(!isSimpleCycle<int>(g, {1, 2, 3, 1, 2, 3, 1}));  // goes around twice

    // --- too short
    CHECK(!isSimpleCycle<int>(g, {}));
    CHECK(!isSimpleCycle<int>(g, {1}));
    CHECK(!isSimpleCycle<int>(g, {1, 1}));
    CHECK(!isSimpleCycle<int>(g, {10, 10}));        // self-loop
    CHECK(!isSimpleCycle<int>(g, {1, 2, 1}));       // there and back

    // --- node not in graph
    CHECK(!isSimpleCycle<int>(g, {1, 2, 99, 1}));
    CHECK(!isSimpleCycle<int>(g, {99, 98, 97, 99}));

    // Undirected graph (each edge stored both ways): square 1-2-3-4-1
    G<int> u;
    for (auto [a, b] : std::vector<std::pair<int, int>>{{1, 2}, {2, 3}, {3, 4}, {4, 1}}) {
        u.addEdge(a, b);
        u.addEdge(b, a);
    }
    CHECK(isSimpleCycle<int>(u, {1, 2, 3, 4, 1}));
    CHECK(isSimpleCycle<int>(u, {1, 4, 3, 2, 1}));  // both directions work
    CHECK(!isSimpleCycle<int>(u, {1, 2, 3, 2, 1})); // backtracks over 2
    CHECK(!isSimpleCycle<int>(u, {1, 3, 4, 1}));    // 1-3 is a diagonal, not an edge

    // Removing an edge breaks the cycle
    u.deleteEdge(3, 4);
    CHECK(!isSimpleCycle<int>(u, {1, 2, 3, 4, 1}));
    CHECK(isSimpleCycle<int>(u, {1, 4, 3, 2, 1}));  // 4->3 still exists
}

// ---------------------------------------------------------------------------
// Part 3: Kevin Bacon
// ---------------------------------------------------------------------------
template <template <class> class G>
void testKevinBacon() {
    G<std::string> social;
    buildSocialGraph(social);
    CHECK(social.getNodes().size() == 20);
    CHECK(social.hasNode(ME));
    CHECK(social.hasNode(KEVIN_BACON));

    // Me -> Kevin Bacon
    std::list<std::string> path;
    CHECK(connectedToKevinBacon(social, path));
    CHECK(!path.empty());
    CHECK(path.front() == ME);
    CHECK(path.back() == KEVIN_BACON);
    CHECK(isValidPath(social, path));
    CHECK(kevinBaconNumber(path) == 6);  // Ethan-Jordan-Olivia-Grace-Meg-(Hanks|Cruise)-Bacon
    CHECK(kevinBaconNumber(path) <= 7);  // "seven degrees"

    // Kevin Bacon's own number is 0
    CHECK(connectedToKevinBacon(social, path, KEVIN_BACON));
    CHECK(path == std::list<std::string>{KEVIN_BACON});
    CHECK(kevinBaconNumber(path) == 0);

    // A co-star is 1 away
    CHECK(connectedToKevinBacon(social, path, "Jack Nicholson"));
    CHECK(kevinBaconNumber(path) == 1);

    // Someone in the separate group is not connected -> false, empty path
    path = {"leftover", "data"};
    CHECK(!connectedToKevinBacon(social, path, "Zoe Adams"));
    CHECK(path.empty());
    CHECK(kevinBaconNumber(path) == -1);

    // Someone not in the graph at all
    CHECK(!connectedToKevinBacon(social, path, "Nobody"));
    CHECK(path.empty());

    // Cut the shortest route (Olivia - Grace); the path goes through Liam instead
    social.deleteEdge("Olivia Hart", "Grace Turner");
    social.deleteEdge("Grace Turner", "Olivia Hart");
    CHECK(connectedToKevinBacon(social, path));
    CHECK(isValidPath(social, path));
    CHECK(contains(std::vector<std::string>(path.begin(), path.end()),
                   std::string("Liam Foster")));
    CHECK(kevinBaconNumber(path) == 7);

    // Remove Grace (the only link to Hollywood): now I'm disconnected
    social.removeNode("Grace Turner");
    CHECK(!connectedToKevinBacon(social, path));
    CHECK(path.empty());

    // ...until I make a new friend who knows Tom Hanks
    social.addEdge(ME, "Tom Hanks");
    social.addEdge("Tom Hanks", ME);
    CHECK(connectedToKevinBacon(social, path));
    CHECK(kevinBaconNumber(path) == 2);
    CHECK((path == std::list<std::string>{ME, "Tom Hanks", KEVIN_BACON}));

    // The friend ring in the separate group is a simple cycle (ties Part 2 to Part 3)
    G<std::string> fresh;
    buildSocialGraph(fresh);
    CHECK(isSimpleCycle<std::string>(
        fresh, {"Zoe Adams", "Ben Carter", "Chloe Nguyen", "Ryan Scott", "Ava Morales", "Zoe Adams"}));
    CHECK(!isSimpleCycle<std::string>(
        fresh, {"Ethan Li", "Maya Chen", "Sam Rivera", "Ethan Li"}));  // Sam isn't Ethan's friend
}

// ---------------------------------------------------------------------------
template <template <class> class G>
void runCommonSuite(const std::string& name) {
    g_suite = name;
    const int before = g_failed;
    testBasicADT<G>();
    testTraversals<G>();
    testIsSimpleCycle<G>();
    testKevinBacon<G>();
    std::cout << (g_failed == before ? "  PASS  " : "  FAIL  ") << name << "\n";
}

int main() {
    std::cout << "Graph library tests\n";
    runCommonSuite<AdjListGraph>("AdjListGraph");
    runCommonSuite<AdjMatrixGraph>("AdjMatrixGraph");
    runCommonSuite<WeightedAdjListGraph>("WeightedAdjListGraph");
    runCommonSuite<WeightedAdjMatrixGraph>("WeightedAdjMatrixGraph");

    g_suite = "WeightedAdjListGraph (weights)";
    testWeighted<WeightedAdjListGraph>();
    g_suite = "WeightedAdjMatrixGraph (weights)";
    testWeighted<WeightedAdjMatrixGraph>();

    std::cout << "\n" << g_passed << " checks passed, " << g_failed << " failed\n";
    return g_failed == 0 ? 0 : 1;
}
