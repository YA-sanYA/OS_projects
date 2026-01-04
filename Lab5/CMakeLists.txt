set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_minimum_required(VERSION 3.10)
project("OS_lab5")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Поддиректории
add_subdirectory(Common)
add_subdirectory(Server)
add_subdirectory(Client)
add_subdirectory(Test)

enable_testing()