#include <iostream>
#include <vector>
#include <string>
#include "pipe_utils.h"
#include "server_utils.h"

int main() {
    setlocale(LC_ALL, "Russian");

    std::string filename;
    std::cout << "Введите имя файла: ";
    std::cin >> filename;

    int n;
    std::cout << "Введите кол-во сотрудников: ";
    std::cin >> n;

    std::vector<employee> database(n);
    createBinaryFile(filename, database);
    printFile(filename);

    int clientCount;
    std::cout << "Введите кол-во клиентов: ";
    std::cin >> clientCount;

    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(NULL, buf, MAX_PATH);
    std::wstring path(buf);
    path = path.substr(0, path.find_last_of(L"\\/")) + L"\\Client.exe";

    startClients(clientCount, path);

    LockManager lockManager;
    std::vector<HANDLE> threads;

    for (int i = 0; i < clientCount; i++) {
        HANDLE hPipe = createServerPipe();
        if (ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
            PipeContext* ctx = new PipeContext{ hPipe, database, &lockManager };
            HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)handleClient, ctx, 0, NULL);
            threads.push_back(hThread);
        }
    }

    if (!threads.empty()) {
        WaitForMultipleObjects((DWORD)threads.size(), threads.data(), TRUE, INFINITE);
        for (HANDLE h : threads) CloseHandle(h);
    }

    std::cout << "\nФинальное состояние файла:\n";
    saveFile(filename, database);
    printFile(filename);

    std::cout << "Сервер завершен. Нажмите Enter.";
    std::cin.get(); std::cin.get();
    return 0;
}