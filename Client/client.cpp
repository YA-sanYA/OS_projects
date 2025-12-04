#include <iostream>
#include <windows.h>
#include <conio.h> // ƒÎˇ _getch(), ˜ÚÓ·˚ "Press any key"
#include "common.h"

int main()
{
    HANDLE hPipe;

    while (true)
    {
        hPipe = CreateFileA(
            PIPE_NAME,
            GENERIC_READ | GENERIC_WRITE,
            0, NULL, OPEN_EXISTING,
            0, NULL
        );

        if (hPipe != INVALID_HANDLE_VALUE) break;

        if (GetLastError() != ERROR_PIPE_BUSY) {
            std::cout << "Could not open pipe. Server might not be running.\n";
            Sleep(2000);
            return 1;
        }

        if (!WaitNamedPipeA(PIPE_NAME, 2000)) {
            std::cout << "Could not open pipe: 2 second wait timed out.\n";
            return 1;
        }
    }

    std::cout << "Connected to server.\n";

    Request req;
    Response res;
    DWORD rwBytes;

    while (true)
    {
        std::cout << "\nChoose option:\n1 - Modify record\n2 - Read record\n3 - Exit\n> ";
        int choice;
        std::cin >> choice;

        if (choice == 3)
        {
            req.type = RequestType::EXIT;
            WriteFile(hPipe, &req, sizeof(req), &rwBytes, nullptr);
            break;
        }

        std::cout << "Enter Employee ID: ";
        std::cin >> req.id;

        // --- ÃŒƒ»‘» ¿÷»ﬂ ---
        if (choice == 1)
        {
            req.type = RequestType::START_MODIFY;
            if (!WriteFile(hPipe, &req, sizeof(req), &rwBytes, nullptr)) break;

            if (!ReadFile(hPipe, &res, sizeof(res), &rwBytes, nullptr)) break;

            if (res.success)
            {
                std::cout << "\nCurrent Data:\n";
                std::cout << "ID: " << res.data.num << " | Name: " << res.data.name << " | Hours: " << res.data.hours << "\n";

                std::cout << "Enter New Name: ";
                std::cin >> req.data.name;
                std::cout << "Enter New Hours: ";
                std::cin >> req.data.hours;
                req.data.num = req.id;

                req.type = RequestType::UPDATE_DATA;
                WriteFile(hPipe, &req, sizeof(req), &rwBytes, nullptr);
                ReadFile(hPipe, &res, sizeof(res), &rwBytes, nullptr);

                if (res.success) std::cout << "Record updated successfully.\n";
                else std::cout << "Update failed.\n";
            }
            else
            {
                std::cout << "Error: " << res.message << "\n";
            }

            std::cout << "Press any key to finish access...";
            _getch();
            req.type = RequestType::RELEASE;
            WriteFile(hPipe, &req, sizeof(req), &rwBytes, nullptr);
        }

        // --- ◊“≈Õ»≈ ---
        else if (choice == 2)
        {
            req.type = RequestType::START_READ;
            WriteFile(hPipe, &req, sizeof(req), &rwBytes, nullptr);
            ReadFile(hPipe, &res, sizeof(res), &rwBytes, nullptr);

            if (res.success)
            {
                std::cout << "ID: " << res.data.num << " | Name: " << res.data.name << " | Hours: " << res.data.hours << "\n";
            }
            else
            {
                std::cout << "Error: " << res.message << "\n";
            }

            std::cout << "Press any key to finish reading...";
            _getch();

            req.type = RequestType::RELEASE;
            WriteFile(hPipe, &req, sizeof(req), &rwBytes, nullptr);
        }
    }

    CloseHandle(hPipe);
    return 0;
}