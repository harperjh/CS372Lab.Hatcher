# CS372Lab.Hatcher
Due: Tue Aug 25,2026 at 11:59PM
Assignment: Timing the Towers of Hanoi
Introductio
The purpose os this assignment wasto compare recursive and iteraive
Twoers of Hanoi programs. Iused the C++ chronon library to measuer thier runing times.

Method
I tested both programs with 5m 10 ,15 ,20, 25, 30, and 35 disks. The time was recorede
before and after each program ran. Printing the moves was tured off so it would not affect the results.

Results
the number of mover is 2^n - 1 as the number of disks increased, the runing time also increased.

Discussion and Conclusion
The recursive program uses function clls, while the iterative program uses stacks. The interative program cam
be faster because it does not need as many recursive function call.However, both programs get much sloer when more
disks are added because the number of moves increases very quickly. Overall, the experiment showed that both programs
take longers as the number of disks increases. The iterative version cam be fater, but the lage number of moves is the main
reason the programs become slow.