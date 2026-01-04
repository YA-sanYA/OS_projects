#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include "EmployeeService.h"

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    if (argc != 4) return 1;

    std::string binFile = argv[1];
    std::string reportFile = argv[2];
    double rate = std::stod(argv[3]);

    auto data = EmployeeService::loadAllEmployees(binFile);
    if (data.empty()) {
        std::cerr << "Нет данных для отчета.\n";
        return 1;
    }

    std::ofstream fout(reportFile);
    fout << "Отчет по файлу " << binFile << ":\n";
    fout << std::left << std::setw(10) << "ID" << std::setw(15) << "Имя"
        << std::setw(10) << "Часы" << std::setw(15) << "Зарплата" << "\n";

    for (const auto& e : data) {
        fout << std::left << std::setw(10) << e.num
            << std::setw(15) << e.name
            << std::setw(10) << e.hours
            << std::setw(15) << EmployeeService::calculateSalary(e.hours, rate) << "\n";
    }

    fout.close();
    std::cout << "Отчет готов.\n";
    return 0;
}