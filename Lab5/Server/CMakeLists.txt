cmake_minimum_required(VERSION 3.10)

project(Server)

add_executable(Server
    server.cpp
)

# Подключаем библиотеку Common
target_link_libraries(Server PRIVATE Common)
target_include_directories(Server PRIVATE ../Common)
