cmake_minimum_required(VERSION 3.13)
project(API_list C)

set(CMAKE_C_STANDARD 99)

add_executable(API_list main.c lab5_dlist.h lab5_dlist.c list_t.h list_t.c logic.h logic.c)