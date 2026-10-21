# Builds and runs the Tree unit tests with GoogleTest.
# GoogleTest is downloaded automatically the first time you configure.
cmake_minimum_required(VERSION 3.14)
project(TreeTests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Visual Studio: use the same C runtime as our test program
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(treetests TreeFixture.cpp TreeParentTests.cpp TreeReviewTests.cpp TreeAlgorithmsTests.cpp)
target_include_directories(treetests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../include/Growing a Tree")
target_link_libraries(treetests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(treetests)
