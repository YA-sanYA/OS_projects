cmake_minimum_required(VERSION 3.15)

# Создаём исполняемый файл Sender
add_executable(Sender sender.cpp)

# Подключаем библиотеку utils
target_link_libraries(Sender PRIVATE utils)
