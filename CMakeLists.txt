cmake_minimum_required(VERSION 3.15)
project("OS_lab4")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Поддиректории
add_subdirectory(Utils)
add_subdirectory(Receiver)
add_subdirectory(Sender)
# add_subdirectory(Test)
