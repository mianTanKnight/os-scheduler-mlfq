cmake_minimum_required(VERSION 3.29)
project(os_scheduler C)

set(CMAKE_C_STANDARD 11)

add_executable(os_scheduler main.c
        mlfq.c
        queue_.c
        task_.c
        timer.c)
