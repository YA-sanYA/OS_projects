#include <iostream>
#include <windows.h>
#include <string>
#include <conio.h>
#include "common.h"

int main() {
    HANDLE hPipe;
    while (true) {
        hPipe = CreateFileW(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hPipe != INVALID_HANDLE_VALUE) break;
        if (!WaitNamedPipeW(PIPE_NAME, 5000)) return 1;
    }

    while (true) {
        std::cout << "\n1 - Modify, 2 - Read, 3 - Exit: ";
        int choice; std::cin >> choice;
        if (choice == 3) break;

        Request req;
        Response res;
        DWORD cb;

        std::cout << "Enter Employee ID: ";
        std::cin >> req.id;

        req.type = (choice == 1) ? RequestType::START_MODIFY : RequestType::START_READ;
        WriteFile(hPipe, &req, sizeof(req), &cb, NULL);
        ReadFile(hPipe, &res, sizeof(res), &cb, NULL);

        if (!res.success) {
            std::cout << "Error: " << res.message << std::endl;
            continue;
        }

        std::cout << "ID: " << res.data.num << " Name: " << res.data.name << " Hours: " << res.data.hours << std::endl;

        if (choice == 1) {
            std::cout << "New Name: ";
            std::string n; std::cin >> n;
            strncpy_s(req.data.name, n.c_str(), _TRUNCATE);

            std::cout << "New Hours: "; std::cin >> req.data.hours;
            req.data.num = req.id;
            req.type = RequestType::UPDATE_DATA;

            WriteFile(hPipe, &req, sizeof(req), &cb, NULL);
            ReadFile(hPipe, &res, sizeof(res), &cb, NULL);
            std::cout << (res.success ? "Updated." : "Failed.") << std::endl;
        }

        std::cout << "Press any key to release record...";
        _getch();
        req.type = RequestType::RELEASE;
        WriteFile(hPipe, &req, sizeof(req), &cb, NULL);
    }

    CloseHandle(hPipe);
    return 0;
}