#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <vector>
#include "employee.h"
#include "EmployeeService.h"

bool runProcess(std::wstring cmdLine) {
    STARTUPINFOW si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);
    if (!CreateProcessW(NULL, &cmdLine[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) return false;
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

int main() {
    setlocale(LC_ALL, "Russian");

    std::wstring binFile, reportFile, count, rate;

    std::wcout << L"Введите имя бинарного файла: ";
    std::wcin >> binFile;
    std::wcout << L"Введите кол-во записей: ";
    std::wcin >> count;

    std::wstring creatorCmd = L"Creator.exe " + binFile + L" " + count;
    runProcess(creatorCmd);

    auto emps = EmployeeService::loadAllEmployees(std::string(binFile.begin(), binFile.end()));
    for (const auto& e : emps) {
        std::cout << "ID: " << e.num << ", Name: " << e.name << ", Hours: " << e.hours << "\n";
    }

    std::wcout << L"Имя файла отчета: ";
    std::wcin >> reportFile;
    std::wcout << L"Ставка: ";
    std::wcin >> rate;

    std::wstring reporterCmd = L"Reporter.exe " + binFile + L" " + reportFile + L" " + rate;
    runProcess(reporterCmd);

    std::ifstream rf(reportFile);
    std::string line;
    while (std::getline(rf, line)) std::cout << line << "\n";

    system("pause");
    return 0;
}