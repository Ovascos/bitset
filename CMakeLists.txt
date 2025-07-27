cmake_minimum_required(VERSION 3.31)
project(bitset)

set(CMAKE_CXX_STANDARD 20)

add_executable(bitset main.cpp
        bitset.cpp
        bitset.hpp)
