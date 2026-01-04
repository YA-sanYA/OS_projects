#include "pipe_utils.h"
#include <fstream>
#include <iostream>

void createBinaryFile(const std::string& filename, std::vector<employee>& database) {
    for (size_t i = 0; i < database.size(); ++i) {
        std::cout << "Employee " << i + 1 << ":\n";
        std::cout << "  ID: "; std::cin >> database[i].num;
        std::cout << "  Name: "; std::cin >> database[i].name;
        std::cout << "  Hours: "; std::cin >> database[i].hours;
    }
    saveFile(filename, database);
}

void saveFile(const std::string& filename, const std::vector<employee>& database) {
    std::ofstream fout(filename, std::ios::binary);
    if (fout.is_open()) {
        fout.write(reinterpret_cast<const char*>(database.data()), database.size() * sizeof(employee));
        fout.close();
    }
}

void printFile(const std::string& filename) {
    std::ifstream fin(filename, std::ios::binary);
    if (!fin) return;
    employee tmp;
    std::cout << "\n--- Содержимое файла ---\n";
    while (fin.read(reinterpret_cast<char*>(&tmp), sizeof(tmp))) {
        std::cout << "ID: " << tmp.num << ", Name: " << tmp.name << ", Hours: " << tmp.hours << "\n";
    }
    fin.close();
}

HANDLE createServerPipe() {
    return CreateNamedPipeW(
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

void startClients(int clientCount, const std::wstring& clientPath) {
    for (int i = 0; i < clientCount; i++) {
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        std::wstring cmd = L"\"" + clientPath + L"\"";
        std::vector<wchar_t> cmdBuf(cmd.begin(), cmd.end());
        cmdBuf.push_back(0);

        if (CreateProcessW(NULL, cmdBuf.data(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    }
}