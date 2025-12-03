cmake_minimum_required(VERSION 3.10)

project(Client)

add_executable(Client
    client.cpp
)

# Подключаем библиотеку Common
target_link_libraries(Client PRIVATE Common)
target_include_directories(Client PRIVATE ../Common)
