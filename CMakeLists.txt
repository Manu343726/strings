cmake_minimum_required(VERSION 3.2)
project(strings)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES strings/main.cpp)
add_executable(strings ${SOURCE_FILES})