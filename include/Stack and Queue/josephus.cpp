#include <iostream>
#include <vector>
#include <stdexcept>
#include "Queue.hpp"

int josephus(int n, int m, std::vector<int>& eliminationOrder) {
    // Reject input that doesn't describe a real game.
    if (n < 1) throw std::invalid_argument("need at least one player");
    if (m < 0) throw std::invalid_argument("number of passes cannot be negative");

    // Seat players 1..n in the circle. Player 1 ends up at the front,
    // holding the potato.
    Queue<int> circle;
    for (int player = 1; player <= n; ++player)
        circle.push(player);

    // Start with an empty result, in case the caller's vector had old data.
    eliminationOrder.clear();

    // Queue has no size() method, so we count the players left ourselves.
    int remaining = n;

    // Keep eliminating people until only the winner is left.
    while (remaining > 1) {
        // Skip full trips around the circle; they don't change who ends up
        int passes = m % remaining;
        for (int i = 0; i < passes; ++i) {
            circle.push(circle.front());
            circle.pop();
        eliminationOrder.push_back(circle.front());
        circle.pop();
        --remaining;
    }

    // The only person left in the queue is the winner.
    int winner = circle.front();
    circle.pop();
    return winner;
}

int main() {
    // Ask the user for the size of the game.
    int n, m;
    std::cout << "Number of players (N): ";
    std::cin >> n;
    std::cout << "Number of passes (M): ";
    std::cin >> m;

    try {
        // Solve the game.
        std::vector<int> order;
        int winner = josephus(n, m, order);

        // Print the elimination order as a comma-separated list.
        // With only one player, nobody is eliminated.
        std::cout << "Elimination order: ";
        if (order.empty()) std::cout << "(none)";
        for (size_t i = 0; i < order.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << order[i];
        }

        // Print the winner.
        std::cout << "\nWinner: " << winner << '\n';
    }
    catch (const std::exception& e) {
        // Bad input
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}