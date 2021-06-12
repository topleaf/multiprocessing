cmake_minimum_required(VERSION 3.14)
project(multiprocessing C)

set(CMAKE_C_STANDARD 11)

add_executable(multiprocessing
        main.c)
#SET(CMAKE_C_COMPILER "/usr/bin/gcc")
#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -pthread ")  # add pthread library

find_package(Threads REQUIRED)
target_link_libraries(multiprocessing Threads::Threads)
#link_libraries(pthread)
