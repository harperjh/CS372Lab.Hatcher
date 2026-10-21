// Quick regression test for the smart-pointer List.
// Build (MSVC): cl /std:c++17 /EHsc /Zi list_regression_test.cpp
#include "List.h"
#include <cassert>
#include <vector>
#include <string>

template <typename T>
std::vector<T> toVec(List<T>& l) {
	std::vector<T> v;
	l.traverse([&](T& x) { v.push_back(x); });
	return v;
}

int main() {
	// empty / single
	List<int> a;
	assert(a.empty() && a.size() == 0);
	List<int> one(7);
	assert(one.size() == 1 && one.front() == 7 && one.back() == 7);

	// push_front / push_back
	a.push_back(2); a.push_back(3); a.push_front(1);
	assert((toVec(a) == std::vector<int>{1, 2, 3}));
	assert(a.front() == 1 && a.back() == 3 && a.size() == 3);

	// pop_back / pop_front down to empty, then reuse
	a.pop_back();  assert((toVec(a) == std::vector<int>{1, 2}));
	a.pop_front(); assert((toVec(a) == std::vector<int>{2}));
	a.pop_back();  assert(a.empty() && a.size() == 0);
	a.pop_back();  // empty: prints warning, must not crash
	a.pop_front();
	a.push_back(9); assert(a.front() == 9 && a.back() == 9);
	a.pop_front(); assert(a.empty());

	// copy ctor / copy assign are deep
	List<int> b; for (int i = 0; i < 5; ++i) b.push_back(i);
	List<int> c(b);
	c.front() = 100;
	assert(b.front() == 0 && c.front() == 100 && c.size() == 5);
	List<int> d; d.push_back(42);
	d = b;
	assert((toVec(d) == toVec(b)));
	d = d;  // self-assignment
	assert(d.size() == 5);

	// move ctor / move assign
	List<int> e(std::move(c));
	assert(e.size() == 5 && c.empty() && c.size() == 0);
	List<int> f; f.push_back(1);
	f = std::move(e);
	assert(f.size() == 5 && f.front() == 100 && e.empty());
	e.push_back(3); assert(e.size() == 1);  // moved-from list still usable

	// back pointers stay correct: drain from the back
	List<int> g; for (int i = 0; i < 4; ++i) g.push_back(i);
	for (int i = 3; i >= 0; --i) { assert(g.back() == i); g.pop_back(); }
	assert(g.empty());

	// insert_before_first: middle, front, back, empty
	List<int> s;
	auto greater = [](int v) { return [v](const int& x) { return x > v; }; };
	s.insert_before_first(greater(5), 5);                 // empty
	s.insert_before_first(greater(1), 1);                 // front
	s.insert_before_first(greater(9), 9);                 // back
	s.insert_before_first(greater(3), 3);                 // middle
	s.insert_before_first(greater(7), 7);                 // middle
	assert((toVec(s) == std::vector<int>{1, 3, 5, 7, 9}) && s.size() == 5);
	for (int i : {9, 7, 5, 3, 1}) { assert(s.back() == i); s.pop_back(); }

	// iterators: mutable, const, range-for, empty list
	List<int> it; for (int i = 1; i <= 4; ++i) it.push_back(i);
	std::vector<int> seen;
	for (auto p = it.begin(); p != it.end(); ++p) { seen.push_back(*p); *p *= 10; }
	assert((seen == std::vector<int>{1, 2, 3, 4}));
	seen.clear();
	for (auto p = it.cbegin(); p != it.cend(); ++p) seen.push_back(*p);
	assert((seen == std::vector<int>{10, 20, 30, 40}));
	const List<int>& cit = it; int sum = 0;
	for (int x : cit) sum += x;
	assert(sum == 100);
	List<int> none; assert(none.begin() == none.end() && none.cbegin() == none.cend());

	// non-trivial type
	List<std::string> str; str.push_back("b"); str.push_front("a");
	assert(str.front() == "a" && str.back() == "b");

	// long list: iterative destruction must not overflow the stack
	{
		List<int> big;
		for (int i = 0; i < 1'000'000; ++i) big.push_back(i);
		assert(big.size() == 1'000'000);
		List<int> big2 = std::move(big);
		big2 = List<int>(1);
	}

	std::cout << "All List regression tests passed.\n";
	return 0;
}