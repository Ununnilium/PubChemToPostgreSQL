cmake_minimum_required(VERSION 3.3)
project(filterCasNumbers)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -march=native")

set(SOURCE_FILES main.cpp)
add_executable(filterCasNumbers ${SOURCE_FILES})

