cmake_minimum_required(VERSION 3.3)
project(filterCasNumbers)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast -march=native")

set(SOURCE_FILES main.c)
add_executable(filterCasNumbers ${SOURCE_FILES})

