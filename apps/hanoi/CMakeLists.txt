cmake_minimum_required(VERSION 3.10)

#set the project name
project(hanoi)

set(CMAKE_CXX_STANDARD 17)

include_directories(../../include)
include_directories(../../include/ReceiptBag)

#add the executable
add_executable(hanoi "../../include/hanoi.cpp")

add_executable(hanoiit "../../include/hanoiit.cpp")

add_executable(receiptbagtest "../../include/ReceiptBagTest.cpp")