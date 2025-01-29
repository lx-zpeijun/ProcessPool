cmake_minimum_required(VERSION 3.21)
project(process_pool C)

set(CMAKE_C_STANDARD 99)

add_executable(process_pool main.c process_pool.c process_pool.h list.c list.h)
