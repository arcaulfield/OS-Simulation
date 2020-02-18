cmake_minimum_required(VERSION 3.15)
project(Assignment2 C)

set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR})
add_executable(mykernel shell.c interpreter.c shellmemory.c ram.c kernel.c pcb.c)