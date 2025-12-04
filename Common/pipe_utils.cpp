#include "pipe_utils.h"
#include <fstream>
#include <iostream>

void createBinaryFile(const std::string& filename, std::vector<employee>& database)
{
    for (size_t i = 0; i < database.size(); ++i)
    {
        std::cout << "Employee " << i + 1 << ":\n";
        std::cout << "  ID: ";
        std::cin >> database[i].num;
        std::cout << "  Name: ";
        std::cin >> database[i].name;
        std::cout << "  Hours: ";
        std::cin >> database[i].hours;
    }
    saveFile(filename, database);
}

void saveFile(const std::string& filename, const std::vector<employee>& database)
{
    std::ofstream fout(filename, std::ios::binary);
    if (fout.is_open()) {
        fout.write(reinterpret_cast<const char*>(database.data()), database.size() * sizeof(employee));
        fout.close();
    }
    else {
        std::cerr << "Error writing to file!\n";
    }
}

void printFile(const std::string& filename)
{
    std::ifstream fin(filename, std::ios::binary);
    if (!fin) {
        std::cout << "File not found.\n";
        return;
    }

    employee tmp;
    std::cout << "\n--- File content (" << filename << ") ---\n";
    while (fin.read(reinterpret_cast<char*>(&tmp), sizeof(tmp)))
    {
        std::cout << "ID: " << tmp.num << ", Name: " << tmp.name << ", Hours: " << tmp.hours << "\n";
    }
    std::cout << "-------------------------\n";
    fin.close();
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

        char cmdLine[] = "..\\Client\\Client.exe";

        if (!CreateProcessA(
            nullptr, cmdLine, nullptr, nullptr, FALSE,
            CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi))
        {
            std::cout << "Failed to start Client.exe. Make sure it is compiled and in the same folder.\n";
            continue;
        }

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}