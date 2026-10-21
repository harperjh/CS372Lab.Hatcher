//
// File:   TreeParentTests.cpp
// Purpose:
//  Unit tests for the parent pointer added to Tree's Node.
//  Every node must point at its parent; the root's parent is nullptr.
//
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "Tree.h"

// Walks a whole tree and checks that each child's parent is the node above it.
template <typename T>
void expectParentsConsistent(const Tree<T>& t) {
if (t.isEmpty()) return;
if (!t.left().isEmpty()) {
ASSERT_TRUE(t.left().hasParent());
EXPECT_EQ(t.left().parent().root(), t.root());
expectParentsConsistent(t.left());
}
if (!t.right().isEmpty()) {
ASSERT_TRUE(t.right().hasParent());
EXPECT_EQ(t.right().parent().root(), t.root());
expectParentsConsistent(t.right());
}
}

// Test: An empty tree has no path to root.
// Precondition: An empty tree is created.
// Postcondition: pathToRoot() is empty.
TEST(TreeParent, EmptyTree) {
Tree<int> t;
EXPECT_TRUE(t.pathToRoot().empty());
}

// Test: The root of a tree has no parent.
// Precondition: A one-node tree and a multi-node tree are created.
// Postcondition: hasParent() is false for both roots and parent() is empty.
TEST(TreeParent, RootHasNoParent) {
Tree<int> one{ 5 };
EXPECT_FALSE(one.hasParent());
EXPECT_TRUE(one.parent().isEmpty());

Tree<int> many{ 45, 74, 28, 32, 44, 100 };
EXPECT_FALSE(many.hasParent());
EXPECT_TRUE(many.parent().isEmpty());
}

// Test: Every node points at its parent after insertions.
// Precondition: A tree is built with insert().
// Postcondition: For every node, child.parent() is that node.
TEST(TreeParent, ParentsSetOnInsert) {
Tree<int> t;
for (int v : { 50, 30, 70, 20, 40, 60, 80, 35, 65 }) {
t = t.insert(v);
expectParentsConsistent(t);
}
EXPECT_EQ(t.size(), 9u);
}

// Test: Walking from a found node reaches the root along the right path.
// Precondition: Tree {45,74,28,32,44,100}; default comparator puts larger
//               values on the left, so 44 sits under 32 -> 28 -> 45.
// Postcondition: pathToRoot() from 44 is 44, 32, 28, 45.
TEST(TreeParent, PathToRootFromFoundNode) {
Tree<int> t{ 45, 74, 28, 32, 44, 100 };
Tree<int> where;
ASSERT_TRUE(t.find(44, where));
EXPECT_EQ(where.pathToRoot(), (std::vector<int>{ 44, 32, 28, 45 }));

ASSERT_TRUE(t.find(100, where));
EXPECT_EQ(where.pathToRoot(), (std::vector<int>{ 100, 74, 45 }));

ASSERT_TRUE(t.find(45, where));
EXPECT_EQ(where.pathToRoot(), (std::vector<int>{ 45 }));
}

// Test: Parent links climb all the way to the root one step at a time.
// Precondition: A tree with a deep node.
// Postcondition: parent() repeatedly returns the next node up, ending at the root.
TEST(TreeParent, ClimbWithParent) {
Tree<int> t{ 10, 5, 1 };   // greater-on-left: 10 <- 5 (right) <- 1 (right)
Tree<int> where;
ASSERT_TRUE(t.find(1, where));
Tree<int> p = where.parent();
EXPECT_EQ(p.root(), 5);
p = p.parent();
EXPECT_EQ(p.root(), 10);
EXPECT_FALSE(p.hasParent());
}

// Test: Inserting into a tree does not break the parent links of the
//       original tree (immutability must still hold with parent pointers).
// Precondition: t1 exists; t2 = t1.insert(...) while t1 is still alive.
// Postcondition: Both trees have correct parent links.
TEST(TreeParent, OldVersionKeepsCorrectParents) {
Tree<int> t1{ 50, 30, 70, 20, 40 };
Tree<int> t2 = t1.insert(45);
Tree<int> t3 = t2.insert(10);
expectParentsConsistent(t1);
expectParentsConsistent(t2);
expectParentsConsistent(t3);

Tree<int> where;
ASSERT_TRUE(t1.find(20, where));
EXPECT_EQ(where.pathToRoot(), (std::vector<int>{ 20, 30, 50 }));
ASSERT_TRUE(t3.find(10, where));
EXPECT_EQ(where.pathToRoot().back(), 50);
}

// Test: A temporary derived tree going away does not leave the original
//       tree's nodes without parents.
// Precondition: t exists; a temporary t.insert(...) is created and destroyed.
// Postcondition: t's parent links are all still valid.
TEST(TreeParent, TemporaryInsertDoesNotOrphanOriginal) {
Tree<int> t{ 45, 74, 28, 32, 44, 100 };
{
Tree<int> tmp = t.insert(200);
EXPECT_EQ(tmp.size(), 7u);
}
expectParentsConsistent(t);
Tree<int> where;
ASSERT_TRUE(t.find(44, where));
EXPECT_EQ(where.pathToRoot(), (std::vector<int>{ 44, 32, 28, 45 }));
}

// Test: Building a node from the same subtree twice gives each side its own
//       copy, so each child has exactly one parent.
// Precondition: Tree(a, 5, a).
// Postcondition: Parent links are consistent on both sides.
TEST(TreeParent, SameSubtreeOnBothSides) {
Tree<int> a{ 1 };
Tree<int> t(a, 5, a);
EXPECT_EQ(t.size(), 3u);
expectParentsConsistent(t);
}

// Test: Parent pointers work for non-int element types.
TEST(TreeParent, StringTree) {
Tree<std::string> t{ "m", "c", "x", "a" };
expectParentsConsistent(t);
Tree<std::string> where;
ASSERT_TRUE(t.find("a", where));
EXPECT_EQ(where.pathToRoot().back(), "m");
}
