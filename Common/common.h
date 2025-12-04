#pragma once
#include <windows.h>
#include <iostream>

static const char* PIPE_NAME = R"(\\.\pipe\EmployeePipe)";

struct employee
{
    int num;
    char name[10];
    double hours;

    employee() : num(0), hours(0.0) { name[0] = '\0'; }
};

enum class RequestType
{
    START_READ,
    START_MODIFY,
    UPDATE_DATA,
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
    char message[64];
};