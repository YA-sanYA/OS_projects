#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <io.h>
#include <fcntl.h>
#include "message_queue.h"
#include "sync_objects.h"

int wmain() {
    _setmode(_fileno(stdout), _O_U8TEXT);
    _setmode(_fileno(stdin), _O_U8TEXT);

    std::wcout << L"Введите имя файла: ";
    std::wstring wfilename;
    std::getline(std::wcin, wfilename);

    std::wcout << L"Введите вместимость очереди: ";
    int capacity;
    if (!(std::wcin >> capacity)) return 1;

    if (!InitMessageFile(wfilename, capacity)) {
        std::wcout << L"Ошибка инициализации файла." << std::endl;
        return 1;
    }

    std::wcout << L"Введите количество Sender: ";
    int numSenders;
    if (!(std::wcin >> numSenders)) return 1;

    HANDLE hMutex = CreateMainMutex();
    HANDLE hEmpty = CreateEmptySemaphore(capacity, capacity);
    HANDLE hFull = CreateFullSemaphore(0, capacity);

    if (!hMutex || !hEmpty || !hFull) {
        std::wcout << L"Ошибка создания синхро-объектов." << std::endl;
        return 1;
    }

    std::vector<HANDLE> readyEvents;
    for (int i = 0; i < numSenders; ++i) {
        readyEvents.push_back(CreateSenderReadyEvent(i));
    }

    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring currentPath(buffer);
    size_t lastSlash = currentPath.find_last_of(L"\\/");
    std::wstring folderPath = (lastSlash != std::wstring::npos) ? currentPath.substr(0, lastSlash + 1) : L"";
    std::wstring senderPath = folderPath + L"Sender.exe";

    for (int i = 0; i < numSenders; ++i) {
        std::wstring cmd = L"\"" + senderPath + L"\" \"" + wfilename + L"\" " + std::to_wstring(i);

        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        std::vector<wchar_t> cmdMutable(cmd.begin(), cmd.end());
        cmdMutable.push_back(0);

        if (!CreateProcessW(NULL, cmdMutable.data(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            std::wcout << L"Ошибка запуска Sender по пути: " << senderPath << L"\nКод ошибки: " << GetLastError() << std::endl;
            for (HANDLE h : readyEvents) CloseHandle(h);
            return 1;
        }
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }

    std::wcout << L"Ожидание готовности всех Sender...\n";
    WaitForMultipleObjects((DWORD)readyEvents.size(), readyEvents.data(), TRUE, INFINITE);
    for (HANDLE h : readyEvents) CloseHandle(h);

    while (true) {
        std::wcout << L"\n1 - Читать сообщение\n2 - Выход\n> ";
        int choice;
        if (!(std::wcin >> choice)) break;

        if (choice == 1) {
            std::wcout << L"Ожидание сообщения (блокировка)..." << std::endl;
            WaitForSingleObject(hFull, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);

            std::string msg;
            if (ReadMessage(wfilename, msg)) {
                std::wcout << L"Получено: " << std::wstring(msg.begin(), msg.end()) << std::endl;
            }

            ReleaseMutex(hMutex);
            ReleaseSemaphore(hEmpty, 1, NULL);
        }
        else if (choice == 2) {
            break;
        }
    }

    CloseHandle(hMutex);
    CloseHandle(hEmpty);
    CloseHandle(hFull);
    return 0;
}