#pragma once
#include <string>
#include <vector>
#include "employee.h"

namespace EmployeeService {
    // Валидация
    bool isValidName(const std::string& name);
    bool isValidHours(double hours);
    bool isValidID(int id);

    // Расчеты
    double calculateSalary(double hours, double hourlyRate);

    // Работа с файлами
    bool saveEmployee(const std::string& filename, const employee& emp);
    std::vector<employee> loadAllEmployees(const std::string& filename);
}