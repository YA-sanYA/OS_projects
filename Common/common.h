#pragma once
#include <windows.h>

#define PIPE_NAME R"(\\.\pipe\EmployeePipe)"

struct employee
{
    int num;
    char name[10];
    double hours;

    employee() : num(0), hours(0.0) { name[0] = '\0'; }

    employee(int n, const char* nm, double h) : num(n), hours(h)
    {
        strncpy_s(name, nm, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
    }
};

enum class RequestType
{
    READ,
    WRITE,
    RELEASE,
    EXIT
};

struct Request
{
    RequestType type;
    int id;
    employee data;
};

struct Response
{
    bool success;
    employee data;
};
