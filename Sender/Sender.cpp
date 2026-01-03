#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <io.h>
#include <fcntl.h>
#include "message_queue.h"
#include "sync_objects.h"

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

int wmain(int argc, wchar_t* argv[]) {
    _setmode(_fileno(stdout), _O_U8TEXT);
    _setmode(_fileno(stdin), _O_U8TEXT);

    if (argc < 3) {
        std::wcout << L"Ошибка: недостаточно аргументов." << std::endl;
        return 1;
    }

    std::wstring wfilename = argv[1];
    int index = std::stoi(argv[2]);

    HANDLE hMutex = OpenMainMutex();
    HANDLE hEmpty = OpenEmptySemaphore();
    HANDLE hFull = OpenFullSemaphore();
    HANDLE hReady = OpenSenderReadyEvent(index);

    if (!hMutex || !hEmpty || !hFull || !hReady) {
        std::wcout << L"Sender " << index << L": Ошибка открытия объектов синхронизации." << std::endl;
        return 1;
    }

    SetEvent(hReady);

    std::wcout << L"Sender " << index << L" запущен. Файл: " << wfilename << std::endl;

    while (true) {
        std::wcout << L"\n1 - Отправить сообщение\n2 - Выход\n> ";

        std::wstring input;
        if (!std::getline(std::wcin, input)) break;
        if (input.empty()) continue;

        if (input == L"1") {
            std::wcout << L"Введите текст (до 20 символов): ";
            std::wstring wmsg;
            std::getline(std::wcin, wmsg);

            if (wmsg.length() >= MAX_MSG_LEN) {
                wmsg = wmsg.substr(0, MAX_MSG_LEN - 1);
            }

            std::wcout << L"Ожидание места в очереди..." << std::endl;

            WaitForSingleObject(hEmpty, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);

            std::string msg = WStringToString(wmsg);
            if (WriteMessage(wfilename, msg)) {
                std::wcout << L"Сообщение отправлено." << std::endl;
            }
            else {
                std::wcout << L"Ошибка записи в файл." << std::endl;
            }

            ReleaseMutex(hMutex);
            ReleaseSemaphore(hFull, 1, NULL);

        }
        else if (input == L"2") {
            break;
        }
    }

    CloseHandle(hMutex);
    CloseHandle(hEmpty);
    CloseHandle(hFull);
    CloseHandle(hReady);

    return 0;
}