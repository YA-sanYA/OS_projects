#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include "EmployeeService.h"

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    if (argc != 3) {
        std::cerr << "Использование: Creator <имя_файла> <кол-во>\n";
        return 1;
    }

    std::string filename = argv[1];
    int count = std::stoi(argv[2]);

    std::ofstream(filename, std::ios::binary | std::ios::trunc).close();

    for (int i = 0; i < count; i++) {
        employee emp{};
        std::string inputName;

        std::cout << "\nСотрудник #" << (i + 1) << "\nID: ";
        while (!(std::cin >> emp.num) || !EmployeeService::isValidID(emp.num)) {
            std::cout << "Ошибка! Введите ID (число >= 0): ";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }

        std::cout << "Имя (до 9 символов): ";
        std::cin >> inputName;
        while (!EmployeeService::isValidName(inputName)) {
            std::cout << "Ошибка! Имя должно быть от 1 до 9 символов: ";
            std::cin >> inputName;
        }
        strncpy_s(emp.name, inputName.c_str(), _TRUNCATE);

        std::cout << "Часы: ";
        while (!(std::cin >> emp.hours) || !EmployeeService::isValidHours(emp.hours)) {
            std::cout << "Ошибка! Введите часы (>= 0): ";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }

        EmployeeService::saveEmployee(filename, emp);
    }

    std::cout << "Файл создан.\n";
    return 0;
}