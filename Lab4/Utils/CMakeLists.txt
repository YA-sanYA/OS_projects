cmake_minimum_required(VERSION 3.15)

# Создаём статическую библиотеку utils из файлов Utils
add_library(utils STATIC
    message_queue.cpp
    sync_objects.cpp
    message_queue.h
    sync_objects.h
)

# Заголовочные файлы доступны для подключающих проектов
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
