cmake_minimum_required(VERSION 3.12)
project(TechShell C)

set(CMAKE_C_STANDARD 99)

add_executable(TechShell main.c command.c command.h shell.c shell.h)