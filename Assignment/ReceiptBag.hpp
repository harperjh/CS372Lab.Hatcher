#ifndef RECEIPT_BAG_HPP
#define RECEIPT_BAG_HPP

#include <vector> //For storing items and receipts

template <typename T> //Works with any type
class ReceiptBag {
private:
    std::vector<T>   items; //Stores the actual items
    std::vector<int> receipts; // Stores recepit number 
    int nextReceipt; // Next receipt numne give

    int findIndex(int receipt) const; //Find itwm by ticker number

public:
    ReceiptBag(); // Create empty bag

    int insert(const T& item); // Add item-> get receipt
    T remove(int receipt); //Show receipt -> get item back and throws error if recipt invalid
    bool contains(int receipt) const; //Check if receipt is stiil valid
    int size() const; // How may bag
    bool isEmpty() const; // is bag emty
};
#include "ReceiptBag.cpp" // Include the code for the functions

#endif