cmake_minimum_required(VERSION 3.29)
project(Bianyi)

set(CMAKE_CXX_STANDARD 20)


add_executable(experiment1 experiment1/experiment1.cpp)
add_executable(experiment2 experiment2/experiment2.cpp)
add_executable(experiment3 experiment3/experiment3.cpp)

add_executable(test experiment2/test.cpp)