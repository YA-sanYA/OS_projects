cmake_minimum_required(VERSION 3.10)

project(CommonLib)

# Создаём статическую библиотеку
add_library(Common STATIC
    common.h
    locking.cpp
    locking.h
    pipe_utils.cpp
    server_utils.cpp
)

target_include_directories(Common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
