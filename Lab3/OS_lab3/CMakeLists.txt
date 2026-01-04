# CMakeList.txt: проект CMake для OS_lab3; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (OS_lab3 "OS_lab3.cpp" "OS_lab3.h" "OS_lab3_thread.cpp" "OS_lab3_thread.h")

target_link_libraries(OS_lab3 PRIVATE Lib)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET OS_lab3 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
