cmake_minimum_required(VERSION 3.15)
project("OS_lab4")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Чтобы все EXE были в одной папке и видели друг друга
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ВКЛЮЧАЕМ ПОДДЕРЖКУ ТЕСТОВ
enable_testing()

add_subdirectory(Utils)
add_subdirectory(Receiver)
add_subdirectory(Sender)
add_subdirectory(Test)