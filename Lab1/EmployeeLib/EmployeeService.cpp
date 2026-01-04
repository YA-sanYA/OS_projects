#include "EmployeeService.h"
//#include "pch.h"
#include <fstream>
#include <cstring>

namespace EmployeeService {

    bool isValidName(const std::string& name) {
        return !name.empty() && name.length() < 10;
    }

    bool isValidHours(double hours) {
        return hours >= 0;
    }

    bool isValidID(int id) {
        return id >= 0;
    }

    double calculateSalary(double hours, double hourlyRate) {
        if (hours < 0 || hourlyRate < 0) return 0.0;
        return hours * hourlyRate;
    }

    bool saveEmployee(const std::string& filename, const employee& emp) {
        std::ofstream fout(filename, std::ios::binary | std::ios::app);
        if (!fout.is_open()) return false;
        fout.write(reinterpret_cast<const char*>(&emp), sizeof(employee));
        return true;
    }

    std::vector<employee> loadAllEmployees(const std::string& filename) {
        std::vector<employee> result;
        std::ifstream fin(filename, std::ios::binary);
        if (!fin.is_open()) return result;

        employee emp{};
        while (fin.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
            result.push_back(emp);
        }
        return result;
    }
}