//
// File:   TreeReviewTests.cpp
// Purpose:
//  Unit tests for the changes made to Tree in response to the AI code
//  review (see docs/Tree_AI_Review.md).
//
#include <gtest/gtest.h>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "Tree.h"

// Test: The comparator is part of the tree's type.
// Precondition: The same values go into a default tree and a std::less tree.
// Postcondition: Default (std::greater) inorder is descending; std::less is ascending.
TEST(TreeReview, ComparatorIsPartOfTheType) {
Tree<int> descending{ 3, 1, 4, 2 };
Tree<int, std::less<int>> ascending{ 3, 1, 4, 2 };
std::vector<int> d, a;
descending.inorder([&](int v) { d.push_back(v); });
ascending.inorder([&](int v) { a.push_back(v); });
EXPECT_EQ(d, (std::vector<int>{ 4, 3, 2, 1 }));
EXPECT_EQ(a, (std::vector<int>{ 1, 2, 3, 4 }));
EXPECT_TRUE(ascending.member(2));
EXPECT_FALSE(ascending.member(5));
}

// Test: Default ordering puts larger values on the left (matches the fixed comments).
// Precondition: Tree {10, 20, 5}.
// Postcondition: left().root() is 20 and right().root() is 5.
TEST(TreeReview, DefaultOrderingLargerGoesLeft) {
Tree<int> t{ 10, 20, 5 };
EXPECT_EQ(t.left().root(), 20);
EXPECT_EQ(t.right().root(), 5);
}

// Test: Cached size is correct for every subtree.
// Precondition: A tree built by insert(), including a duplicate.
// Postcondition: size() of every subtree equals the number of nodes counted by a traversal.
template <typename Tr>
void expectSizesMatchTraversal(const Tr& t) {
std::size_t counted = 0;
t.preorder([&](int) { ++counted; });
EXPECT_EQ(t.size(), counted);
if (!t.isEmpty()) {
expectSizesMatchTraversal(t.left());
expectSizesMatchTraversal(t.right());
}
}
TEST(TreeReview, CachedSizeMatchesNodeCount) {
Tree<int> t;
for (int v : { 50, 30, 70, 20, 40, 60, 80, 30, 65 }) {   // 30 is a duplicate
t = t.insert(v);
}
EXPECT_EQ(t.size(), 8u);
expectSizesMatchTraversal(t);
}

// Test: Size stays correct for an older version after a new one is built.
// Precondition: t2 = t1.insert(...) while t1 is alive (forces subtree copies).
// Postcondition: Both versions report the right sizes everywhere.
TEST(TreeReview, CachedSizeAcrossVersions) {
Tree<int> t1{ 50, 30, 70 };
Tree<int> t2 = t1.insert(10);
EXPECT_EQ(t1.size(), 3u);
EXPECT_EQ(t2.size(), 4u);
expectSizesMatchTraversal(t1);
expectSizesMatchTraversal(t2);
}

// Test: Operations that need a node throw on an empty tree instead of crashing.
// Precondition: An empty tree.
// Postcondition: root/left/right/parent/hasParent throw std::out_of_range;
//                isEmpty/size/member/find still work.
TEST(TreeReview, EmptyTreeThrows) {
Tree<int> t;
EXPECT_THROW(t.root(), std::out_of_range);
EXPECT_THROW(t.left(), std::out_of_range);
EXPECT_THROW(t.right(), std::out_of_range);
EXPECT_THROW(t.parent(), std::out_of_range);
EXPECT_THROW(t.hasParent(), std::out_of_range);
EXPECT_TRUE(t.isEmpty());
EXPECT_EQ(t.size(), 0u);
EXPECT_FALSE(t.member(1));
Tree<int> where;
EXPECT_FALSE(t.find(1, where));
}

// A value type that counts how many times it is copied.
struct Counted {
int v;
static int copies;
explicit Counted(int x) : v(x) {}
Counted(const Counted& o) : v(o.v) { ++copies; }
Counted& operator=(const Counted& o) { v = o.v; ++copies; return *this; }
bool operator>(const Counted& o) const { return v > o.v; }
};
int Counted::copies = 0;

// Test: Searching and traversing do not copy the stored values.
// Precondition: A tree of Counted values.
// Postcondition: member/find and all three traversals with a const& visitor make 0 copies.
TEST(TreeReview, NoCopiesWhenReading) {
Tree<Counted> t;
for (int i : { 5, 3, 8, 1, 4 }) t = t.insert(Counted(i));
Counted::copies = 0;
EXPECT_TRUE(t.member(Counted(4)));
Tree<Counted> where;
EXPECT_TRUE(t.find(Counted(1), where));
int sum = 0;
auto add = [&](const Counted& c) { sum += c.v; };
t.preorder(add);
t.inorder(add);
t.postorder(add);
EXPECT_EQ(sum, 3 * (5 + 3 + 8 + 1 + 4));
EXPECT_EQ(Counted::copies, 0);
}

// Test: operator<< still prints the preorder walk, and works for std::less trees.
TEST(TreeReview, StreamOutput) {
std::ostringstream a, b, c;
a << Tree<int>{ 2, 3, 1 };
b << Tree<int, std::less<int>>{ 2, 3, 1 };
c << Tree<int>{};
EXPECT_EQ(a.str(), "[2 3 1 ]");
EXPECT_EQ(b.str(), "[2 1 3 ]");
EXPECT_EQ(c.str(), "[]");
}
