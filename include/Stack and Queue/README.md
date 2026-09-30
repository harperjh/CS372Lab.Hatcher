# CS372Lab.Hatcher
Due: Tue Sep 29,2026 at 11:59PM
Assignment:Playing with Queues
The goal of this assignment is to solve the Josephus problem using a queue.
The program takes the total number of players (N) and the step count (M) as input.
I use a queue to store players numbered 1 through N. In each round, the program moves 
the player at the front of the queue to the back—repeating this action M times—and then 
eliminates the next player in line. This process continues until only one player remains.
The program successfully determines the elimination order and the winner. For example, when 
N = 5 and M = 1, the elimination order is 2, 4, 1, 5, and the winner is 3.
Using a queue simplifies the solution to the Josephus problem, as moving the
player at the front to the back effectively simulates the movement of players in
a circle. The program works for various values ??of N and M.