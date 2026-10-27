#include "List.hpp"
#include "CircularList.hpp"
#include "bag.h"
#include <iostream>

int main() {
    List<int> l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);
    std::cout << "front: " << l.front() << ", back: " << l.back() << std::endl;

    CircularList<int> cl;
    cl.push_back(10);
    cl.push_back(20);
    cl.push_back(30);
    cl.traverse(1, [](int& v) { std::cout << v << " "; }); // 20 30 10
    std::cout << std::endl;

    Bag<int> b;
    b.insert(5);
    b.insert(5);
    std::cout << "count of 5: " << b.count(5) << std::endl;
}