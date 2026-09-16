#pragma once
#include <iostream>
#include "List.hpp"
// push_back / pop_back / traverse
template <typename Thing>
class Bag {
public:
	// Add an item to the bag.
	void insert(Thing aThing) {
		bagContents.push_back(aThing);
	}
	// Remove and return an item from the bag.
	// return a reference to something that's about to be popped.
	Thing &pop() {
		Thing &aThing = *(new Thing());
		if (!bagContents.empty()) {
			aThing = bagContents.back();
			bagContents.pop_back();
		}
		else {
			std::cerr << "Can't pop out of an empty bag" << std::endl;
		}
		return aThing;
	}
	// Number of items currently in the bag.
	int size() {
		return static_cast<int>(bagContents.size());
	}
	// Count how many times aThing appears in the bag.
	int count(Thing aThing) {
		int bagCount = 0;
		bagContents.traverse([&](Thing &item) {
			if (item == aThing) {
				bagCount++;
			}
		});
		return bagCount;
	}
private:
	List<Thing> bagContents; // backing store: a doubly linked
};
