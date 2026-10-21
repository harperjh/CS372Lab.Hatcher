//
// File:   TreeAlgorithmsTests.cpp
// Purpose:
//  Unit tests for the stand-alone levelorder() and prune() functions.
//
#include <gtest/gtest.h>
#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include "TreeAlgorithms.h"

// Collects a level traversal into a vector.
template <typename Tr>
std::vector<int> levels(const Tr& t) {
std::vector<int> out;
levelorder(t, [&](int v) { out.push_back(v); });
return out;
}

// Checks that every child's parent link points at the node above it.
template <typename Tr>
void expectParentsOk(const Tr& t) {
if (t.isEmpty()) return;
for (const Tr& child : { t.left(), t.right() }) {
if (!child.isEmpty()) {
ASSERT_TRUE(child.hasParent());
EXPECT_EQ(child.parent().root(), t.root());
expectParentsOk(child);
}
}
}

// ---------------------------------------------------------------- levelorder

// Test: Level order visits each level from top to bottom, left to right.
// Precondition: Tree {45,74,28,32,44,100}. With the default comparator larger
//               values go left, so the levels are
//                 0: 45   1: 74 28   2: 100 32   3: 44
// Postcondition: Visit order is 45 74 28 100 32 44.
TEST(TreeLevelOrder, VisitsLevelByLevel) {
Tree<int> t{ 45, 74, 28, 32, 44, 100 };
EXPECT_EQ(levels(t), (std::vector<int>{ 45, 74, 28, 100, 32, 44 }));
}

// Test: A full three-level tree.
// Precondition: Tree<int, std::less<int>> {50,30,70,20,40,60,80} (smaller left).
// Postcondition: 50 | 30 70 | 20 40 60 80.
TEST(TreeLevelOrder, FullTreeAscendingOrdering) {
Tree<int, std::less<int>> t{ 50, 30, 70, 20, 40, 60, 80 };
EXPECT_EQ(levels(t), (std::vector<int>{ 50, 30, 70, 20, 40, 60, 80 }));
}

// Test: Edge cases.
// Precondition: An empty tree and a one-node tree.
// Postcondition: The empty tree visits nothing; the single node visits only the root.
TEST(TreeLevelOrder, EmptyAndSingleNode) {
EXPECT_TRUE(levels(Tree<int>{}).empty());
EXPECT_EQ(levels(Tree<int>{ 7 }), (std::vector<int>{ 7 }));
}

// Test: A list-shaped (degenerate) tree visits every node, one per level.
// Precondition: Values inserted in sorted order.
// Postcondition: Order is the insertion order.
TEST(TreeLevelOrder, DegenerateTree) {
Tree<int> t{ 1, 2, 3, 4, 5 };   // each larger value goes left
EXPECT_EQ(levels(t), (std::vector<int>{ 1, 2, 3, 4, 5 }));
}

// Free function used as a function-pointer visitor.
static std::vector<int> fpResult;
static void recordValue(int v) { fpResult.push_back(v); }

// A functor (function object) visitor that sums the values it sees.
struct Summer {
int* total;
void operator()(int v) const { *total += v; }
};

// Test: Works with a function pointer and with a functor, not just lambdas.
TEST(TreeLevelOrder, FunctionPointerAndFunctorVisitors) {
Tree<int> t{ 45, 74, 28, 32, 44, 100 };
fpResult.clear();
levelorder(t, recordValue);
EXPECT_EQ(fpResult, (std::vector<int>{ 45, 74, 28, 100, 32, 44 }));

int total = 0;
levelorder(t, Summer{ &total });
EXPECT_EQ(total, 45 + 74 + 28 + 32 + 44 + 100);
}

// Test: Visits every node exactly once and leaves the tree unchanged.
TEST(TreeLevelOrder, VisitsEveryNodeOnceAndDoesNotModify) {
Tree<int> t;
for (int v : { 50, 30, 70, 20, 40, 60, 80, 35, 65, 10 }) t = t.insert(v);
std::vector<int> lv = levels(t), in;
t.inorder([&](int v) { in.push_back(v); });
std::sort(lv.begin(), lv.end());
std::sort(in.begin(), in.end());
EXPECT_EQ(lv, in);
EXPECT_EQ(t.size(), 10u);
}

// Test: Works for non-int element types.
TEST(TreeLevelOrder, StringTree) {
Tree<std::string, std::less<std::string>> t{ "m", "c", "x", "a", "e" };
std::vector<std::string> out;
levelorder(t, [&](const std::string& s) { out.push_back(s); });
EXPECT_EQ(out, (std::vector<std::string>{ "m", "c", "x", "a", "e" }));
}

// --------------------------------------------------------------------- prune

// Test: Prune removes exactly the leaves.
// Precondition: Tree {45,74,28,32,44,100}; its leaves are 100 and 44.
// Postcondition: Result holds 45 74 28 32 (level order) and has size 4.
TEST(TreePrune, RemovesLeaves) {
Tree<int> t{ 45, 74, 28, 32, 44, 100 };
Tree<int> p = prune(t);
EXPECT_EQ(p.size(), 4u);
EXPECT_EQ(levels(p), (std::vector<int>{ 45, 74, 28, 32 }));
EXPECT_FALSE(p.member(100));
EXPECT_FALSE(p.member(44));
}

// Test: The original tree is not modified.
TEST(TreePrune, OriginalUnchanged) {
Tree<int> t{ 45, 74, 28, 32, 44, 100 };
Tree<int> p = prune(t);
EXPECT_EQ(t.size(), 6u);
EXPECT_EQ(levels(t), (std::vector<int>{ 45, 74, 28, 100, 32, 44 }));
expectParentsOk(t);
}

// Test: Full tree loses its whole bottom level.
// Precondition: Full 3-level tree (7 nodes, 4 leaves).
// Postcondition: 3 nodes remain: 50 | 30 70.
TEST(TreePrune, FullTreeLosesBottomLevel) {
Tree<int, std::less<int>> t{ 50, 30, 70, 20, 40, 60, 80 };
auto p = prune(t);
EXPECT_EQ(p.size(), 3u);
EXPECT_EQ(levels(p), (std::vector<int>{ 50, 30, 70 }));
}

// Test: Edge cases.
// Precondition: Empty tree, single node, root with one child.
// Postcondition: Empty and single node prune to empty; root+child keeps only the root.
TEST(TreePrune, EdgeCases) {
EXPECT_TRUE(prune(Tree<int>{}).isEmpty());
EXPECT_TRUE(prune(Tree<int>{ 7 }).isEmpty());
Tree<int> twoNodes{ 10, 20 };
Tree<int> p = prune(twoNodes);
ASSERT_EQ(p.size(), 1u);
EXPECT_EQ(p.root(), 10);
}

// Test: Pruning repeatedly eventually empties the tree, one height at a time.
// Precondition: Degenerate tree of 5 nodes (height 5).
// Postcondition: Each prune removes one node; after 5 prunes the tree is empty.
TEST(TreePrune, RepeatedPruneShrinksToEmpty) {
Tree<int> t{ 1, 2, 3, 4, 5 };
for (std::size_t expected = 4; expected != static_cast<std::size_t>(-1); --expected) {
t = prune(t);
EXPECT_EQ(t.size(), expected);
}
EXPECT_TRUE(t.isEmpty());
}

// Test: The pruned tree is still a valid BST with correct parent pointers.
// Precondition: A larger tree, pruned.
// Postcondition: Inorder of the pruned tree is still ordered (descending for
//                the default comparator); every parent link is correct.
TEST(TreePrune, ResultIsValidBstWithParents) {
Tree<int> t;
for (int v : { 50, 30, 70, 20, 40, 60, 80, 35, 65, 10 }) t = t.insert(v);
Tree<int> p = prune(t);
std::vector<int> in;
p.inorder([&](int v) { in.push_back(v); });
EXPECT_TRUE(std::is_sorted(in.begin(), in.end(), std::greater<int>()));
for (int v : in) EXPECT_TRUE(p.member(v));
expectParentsOk(p);
}
