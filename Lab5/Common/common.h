#pragma once
#include <windows.h>

// Используем L для Unicode строки
inline constexpr wchar_t PIPE_NAME[] = L"\\\\.\\pipe\\EmployeePipe";

struct employee {
    int num;
    char name[10];
    double hours;
};

enum class RequestType {
    START_READ,
    START_MODIFY,
    UPDATE_DATA,
    RELEASE,
    EXIT
};

struct Request {
    RequestType type;
    int id;
    employee data;
};

struct Response {
    bool success;
    employee data;
    char message[64];
};