cmake_minimum_required(VERSION 2.8)
project(Phase3)

set(CMAKE_CXX_FLAGS "-std=c++11")

# Here, create an executable file named 'simulator'
add_executable(simulator main.cpp "readckt.h")