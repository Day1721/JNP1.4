cmake_minimum_required(VERSION 3.6)
project(Zadanie5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cpp virus_genealogy.h)
add_executable(Zadanie5 ${SOURCE_FILES})

add_executable(Zadanie5Test test.cc virus_genealogy.h)