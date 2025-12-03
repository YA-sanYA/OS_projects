#include "pipe_utils.h"
#include <fstream>
#include <iostream>

void createBinaryFile(const std::string& filename, std::vector<employee>& database)
{
    for (size_t i = 0; i < database.size(); ++i)
    {
        std::cout << "Enter ID: ";
        std::cin >> database[i].num;
        std::cout << "Enter name: ";
        std::cin >> database[i].name;
        std::cout << "Enter hours: ";
        std::cin >> database[i].hours;
    }

    std::ofstream fout(filename, std::ios::binary);
    fout.write(reinterpret_cast<char*>(database.data()), database.size() * sizeof(employee));
    fout.close();
}

void printFile(const std::string& filename)
{
    std::ifstream fin(filename, std::ios::binary);
    if (!fin) return;

    std::vector<employee> data;
    employee tmp;
    while (fin.read(reinterpret_cast<char*>(&tmp), sizeof(tmp)))
    {
        data.push_back(tmp);
    }
    fin.close();

    std::cout << "\nFile content:\n";
    for (auto& e : data)
        std::cout << e.num << " " << e.name << " " << e.hours << "\n";
}

HANDLE createServerPipe()
{
    return CreateNamedPipeA(
        PIPE_NAME,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        sizeof(Response),
        sizeof(Request),
        0,
        NULL
    );
}

void startClients(int clientCount)
{
    for (int i = 0; i < clientCount; i++)
    {
        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        if (!CreateProcessA(
            "..\\Client\\Client.exe", nullptr, nullptr, nullptr, FALSE,
            CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi))
        {
            std::cout << "Failed to start client " << i << "\n";
            continue;
        }

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}
