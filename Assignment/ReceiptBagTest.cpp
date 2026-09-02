#include <iostream>
#include <string>
#include "ReceiptBag.hpp"

void testInsertReturnsUniqueReceipts() { //Check if receipt are unique
    ReceiptBag<std::string> bag;
    int r1 = bag.insert("apple");
    int r2 = bag.insert("banana");
    int r3 = bag.insert("cherry");

    bool pass = (r1 != r2) && (r2 != r3) && (r1 != r3) && (bag.size() == 3);
    std::cout << "testInsertReturnsUniqueReceipts: " << (pass ? "PASS" : "FAIL")
        << "  (r1=" << r1 << ", r2=" << r2 << ", r3=" << r3
        << ", size=" << bag.size() << ")\n";
}

void testRemoveReturnsCorrectItem() {  //Remove returns the correct item
    ReceiptBag<std::string> bag;
    int r1 = bag.insert("apple");
    int r2 = bag.insert("banana");
    std::string removed = bag.remove(r1);

    bool pass = (removed == "apple") && (bag.size() == 1)
        && !bag.contains(r1) && bag.contains(r2);
    std::cout << "testRemoveReturnsCorrectItem: " << (pass ? "PASS" : "FAIL")
        << "  (removed=\"" << removed << "\", size=" << bag.size() << ")\n";
}

void testRemoveInvalidReceiptThrows() {  //Removing invalid receipt throws error
    ReceiptBag<int> bag;
    bag.insert(42);
    bool caught = false;
    try {
        bag.remove(9999);  //This receipt doesn't exist
    }
    catch (const std::invalid_argument&) {
        caught = true;  // Good error eas thrown
    }
    std::cout << "testRemoveInvalidReceiptThrows: " << (caught ? "PASS" : "FAIL") << "\n";
}

void testReceiptCannotBeReusedAfterRemoval() {  //Receipt cannot be used twice
    ReceiptBag<int> bag;
    int r1 = bag.insert(100);
    bag.remove(r1);  // Use receipt once
    bool caught = false;
    try {
        bag.remove(r1);  //Try to use same receipt again
    }
    catch (const std::invalid_argument&) {
        caught = true;
    }
    std::cout << "testReceiptCannotBeReusedAfterRemoval: " << (caught ? "PASS" : "FAIL") << "\n";
}

void testEmptyAfterAllRemoved() {  //Bag is empty after removing all items
    ReceiptBag<int> bag;
    int r1 = bag.insert(1);
    int r2 = bag.insert(2);
    bag.remove(r1);
    bag.remove(r2);
    bool pass = bag.isEmpty();
    std::cout << "testEmptyAfterAllRemoved: " << (pass ? "PASS" : "FAIL") << "\n";
}

void testRemoveFromEmptyBagThrows() {  //Removing from empty bag throws error
    ReceiptBag<int> bag; //Empty bag
    bool caught = false;
    try {
        bag.remove(1);  //Try to remove from empty bag
    }
    catch (const std::invalid_argument&) {
        caught = true; //Good error eas thrown
    }
    std::cout << "testRemoveFromEmptyBagThrows: " << (caught ? "PASS" : "FAIL") << "\n";
}

void testDuplicateValuesGetDistinctReceipts() {  //Duplicate items get different receipts
    ReceiptBag<std::string> bag;
    int r1 = bag.insert("apple");
    int r2 = bag.insert("apple"); //Same value, different receipt

    bool pass = (r1 != r2) && (bag.size() == 2);

    std::string removed = bag.remove(r1);
    pass = pass && (removed == "apple") && (bag.size() == 1) && bag.contains(r2) && !bag.contains(r1);

    std::cout << "testDuplicateValuesGetDistinctReceipts: " << (pass ? "PASS" : "FAIL") << "\n";
}

void testReceiptsStayUniqueAfterRemovalAndReinsertion() {  //Receipts stay unique after removals
    ReceiptBag<int> bag;
    int r1 = bag.insert(10);
    int r2 = bag.insert(20);
    bag.remove(r1);
    int r3 = bag.insert(30); //New recipt after removal

    bool pass = (r3 != r1) && (r3 != r2) && bag.contains(r2) && bag.contains(r3) && !bag.contains(r1);
    std::cout << "testReceiptsStayUniqueAfterRemovalAndReinsertion: " << (pass ? "PASS" : "FAIL") << "\n";
}

void testMixedInsertRemoveSequence() {  //Mixed inset and remove sequence works
    ReceiptBag<std::string> bag;
    int rA = bag.insert("A");
    int rB = bag.insert("B");
    int rC = bag.insert("C");

    bag.remove(rB);
    int rD = bag.insert("D");
    bag.remove(rA);

    bool pass = (bag.size() == 2)
        && !bag.contains(rA)
        && !bag.contains(rB)
        && bag.contains(rC)
        && bag.contains(rD);

    std::cout << "testMixedInsertRemoveSequence: " << (pass ? "PASS" : "FAIL")
        << "  (size=" << bag.size() << ")\n";
}

void testSizeTrackedCorrectlyThroughout() {  //Sixe is tracked correctly
    ReceiptBag<int> bag;
    bool pass = (bag.size() == 0) && bag.isEmpty();

    int r1 = bag.insert(1);
    pass = pass && (bag.size() == 1) && !bag.isEmpty();

    int r2 = bag.insert(2);
    pass = pass && (bag.size() == 2);

    bag.remove(r1);
    pass = pass && (bag.size() == 1);

    bag.remove(r2);
    pass = pass && (bag.size() == 0) && bag.isEmpty();

    std::cout << "testSizeTrackedCorrectlyThroughout: " << (pass ? "PASS" : "FAIL") << "\n";
}

void testRemovingOneCopyLeavesOthersIntact() {  //Removing one copy leaves other
    ReceiptBag<int> bag;
    int r1 = bag.insert(100);
    int r2 = bag.insert(100);
    int r3 = bag.insert(100);

    bag.remove(r2);  //Remove middle one

    bool pass = (bag.size() == 2) && bag.contains(r1) && bag.contains(r3) && !bag.contains(r2);
    std::cout << "testRemovingOneCopyLeavesOthersIntact: " << (pass ? "PASS" : "FAIL")
        << "  (size=" << bag.size() << ")\n";
}

int main() {  //Main run all tests
    std::cout << "===== Running ReceiptBag Tests =====\n";
    testInsertReturnsUniqueReceipts();
    testRemoveReturnsCorrectItem();
    testRemoveInvalidReceiptThrows();
    testReceiptCannotBeReusedAfterRemoval();
    testEmptyAfterAllRemoved();
    testRemoveFromEmptyBagThrows();
    testDuplicateValuesGetDistinctReceipts();
    testReceiptsStayUniqueAfterRemovalAndReinsertion();
    testMixedInsertRemoveSequence();
    testSizeTrackedCorrectlyThroughout();
    testRemovingOneCopyLeavesOthersIntact();
    std::cout << "===== All Tests Completed =====\n";
    return 0;
}