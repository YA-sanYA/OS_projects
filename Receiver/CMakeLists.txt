cmake_minimum_required(VERSION 3.15)

# Создаём исполняемый файл Receiver
add_executable(Receiver receiver.cpp)

# Подключаем библиотеку utils
target_link_libraries(Receiver PRIVATE utils)
