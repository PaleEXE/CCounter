cmake_minimum_required(VERSION 3.31)
project(c_lion C)

set(CMAKE_C_STANDARD 23)

add_executable(c_lion main.c
        counter.c
        counter.h
        str.c
        str.h)
