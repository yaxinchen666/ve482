cmake_minimum_required(VERSION 3.13)
project(lemondbfsc C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "-D_FILE_OFFSET_BITS=64")

add_executable(lemondbfsc lemondbfs.c run_query.h run_query.cpp)