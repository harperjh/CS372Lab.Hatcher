#include <iostream>
#include <string>
#include "vector.hpp"
#include "vector_variant.hpp"

void printAll(const Vector<std::string>& v) {
    for (Vector<std::string>::const_iterator it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
}

int main() {
    Vector<int> nums;
    for (int i = 1; i <= 5; ++i) {
        nums.push_back(i * 10);
    }

    std::cout << "Loop with iterator: ";
    for (Vector<int>::iterator it = nums.begin(); it != nums.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    for (Vector<int>::iterator it = nums.begin(); it != nums.end(); it++) {
        *it += 1;
    }
    std::cout << "After *it += 1:     ";
    for (int x : nums) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    Vector<int>::iterator it = nums.begin();
    nums.push_back(60);
    nums.push_back(70);
    std::cout << "Iterator after growth still reads: " << *it << "\n";

    Vector<int> empty;
    std::cout << "Empty vector begin() == end(): "
              << (empty.begin() == empty.end() ? "true" : "false") << "\n";

    Vector<std::string> words;
    words.push_back("feed dog");
    words.push_back("work");
    words.push_back("school");
    std::cout << "Const iteration:    ";
    printAll(words);

    std::cout << "First word length via ->: " << words.begin()->size() << "\n";

    VectorBigStep<int> big;
    for (int i = 1; i <= 3; ++i) {
        big.push_back(i);
    }
    std::cout << "VectorBigStep:      ";
    for (Vector<int>::iterator bit = big.begin(); bit != big.end(); ++bit) {
        std::cout << *bit << " ";
    }
    std::cout << "\n";

    return 0;
}
