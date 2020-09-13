cmake_minimum_required(VERSION 3.13)
project(h3n2 C)

set(CMAKE_C_STANDARD 11)


add_executable(h3n2 mainmenu.c list_t.c list_t.h logic.h logic.c interface.h interface.c)