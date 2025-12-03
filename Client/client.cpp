#include <iostream>
#include <windows.h>
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

        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        Sleep(1000);
    }

    Request req;
    Response res;

    while (true)
    {
        std::cout << "\n1 - Read\n2 - Write\n0 - Exit\n> ";
        int choice;
        std::cin >> choice;

        if (choice == 0)
        {
            req.type = RequestType::EXIT;
            WriteFile(hPipe, &req, sizeof(req), nullptr, nullptr);
            break;
        }

        std::cout << "Enter ID: ";
        std::cin >> req.id;

        if (choice == 1)
        {
            req.type = RequestType::READ;
            WriteFile(hPipe, &req, sizeof(req), nullptr, nullptr);

            ReadFile(hPipe, &res, sizeof(res), nullptr, nullptr);

            if (res.success)
            {
                std::cout << "ID: " << res.data.num
                    << " Name: " << res.data.name
                    << " Hours: " << res.data.hours << "\n";
            }
            else
            {
                std::cout << "Not found\n";
            }

            req.type = RequestType::RELEASE;
            WriteFile(hPipe, &req, sizeof(req), nullptr, nullptr);
        }

        if (choice == 2)
        {
            req.type = RequestType::WRITE;
            std::cout << "New name: ";
            std::cin >> req.data.name;
            std::cout << "New hours: ";
            std::cin >> req.data.hours;
            req.data.num = req.id;

            WriteFile(hPipe, &req, sizeof(req), nullptr, nullptr);

            ReadFile(hPipe, &res, sizeof(res), nullptr, nullptr);

            if (res.success)
                std::cout << "Updated\n";
            else
                std::cout << "Not found\n";

            req.type = RequestType::RELEASE;
            WriteFile(hPipe, &req, sizeof(req), nullptr, nullptr);
        }
    }

    CloseHandle(hPipe);
    return 0;
}
