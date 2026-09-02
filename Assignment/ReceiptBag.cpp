#include "ReceiptBag.hpp"
#include <stdexcept>

//Starts receipt at 1
template <typename T>
ReceiptBag<T>::ReceiptBag() : nextReceipt(1) { 
}
//Find receipt position, reurn -1 if not found
template <typename T>
int ReceiptBag<T>::findIndex(int receipt) const {
    for (std::size_t i = 0; i < receipts.size(); ++i) {
        if (receipts[i] == receipt) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
//Inser item, return new receipt number
template <typename T>
int ReceiptBag<T>::insert(const T& item) {
    int receipt = nextReceipt++;
    items.push_back(item);
    receipts.push_back(receipt);
    return receipt;
}
//Remove item by receipt,throw error if invalid
template <typename T>
T ReceiptBag<T>::remove(int receipt) {
    int idx = findIndex(receipt);
    if (idx == -1) {
        throw std::invalid_argument("ReceiptBag::remove - invalid or already-used receipt");
    }

    T removedItem = items[idx];
    //Swap with last item for last removal
    int lastIdx = static_cast<int>(items.size()) - 1;
    items[idx] = items[lastIdx];
    receipts[idx] = receipts[lastIdx];
    items.pop_back();
    receipts.pop_back();

    return removedItem;
}
//Check if receipt is valid
template <typename T>
bool ReceiptBag<T>::contains(int receipt) const {
    return findIndex(receipt) != -1;
}
//Get number of items
template <typename T>
int ReceiptBag<T>::size() const {
    return static_cast<int>(items.size());
}
//Check if bag is empty
template <typename T>
bool ReceiptBag<T>::isEmpty() const {
    return items.empty();
}