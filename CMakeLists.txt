cmake_minimum_required(VERSION 3.31)
project(bitset)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(test)
add_library(bitset STATIC bitset.cpp bitset.hpp)
