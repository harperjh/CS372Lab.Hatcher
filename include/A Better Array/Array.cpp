#include <iostream>
#include "Array.h"

int main() {
    Array<int, 5> a(5);

    for (size_t i = 0; i < a.size(); ++i) {
        a[i] = static_cast<int>(i * i);
    }

    std::cout << "Array<int,5> contents: ";
    for (size_t i = 0; i < a.size(); ++i) {
        std::cout << a[i] << " ";
    }
    std::cout << "\nsize() = " << a.size() << "\n";

    try {
        Array<int, 5> bad(10);
        (void)bad;
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Caught expected exception: " << e.what() << "\n";
    }

    return 0;
}