#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

#include "message_queue.h"
#include "sync_objects.h"

void print_usage(const char* exe) {
    std::wcout << L"Usage: " << exe << L" <queue_file> <sender_index>\n";
    std::wcout << L"  queue_file    - имя бинарного файла очереди (тот же, что у Receiver)\n";
    std::wcout << L"  sender_index  - индекс этого Sender (0..N-1), нужен для события готовности\n";
}

int wmain(int argc, wchar_t* argv[]) {
    if (argc < 3) {
        print_usage("Sender");
        return 1;
    }

    std::wstring wfilename = argv[1];
    int senderIndex = 0;
    try {
        senderIndex = std::stoi(std::wstring(argv[2]));
    }
    catch (...) {
        std::wcout << L"Invalid sender index\n";
        return 1;
    }

    HANDLE hMutex = OpenMainMutex();
    if (!hMutex) {
        std::wcout << L"OpenMainMutex failed. Убедитесь, что Receiver создал мьютекс.\n";
        return 1;
    }

    HANDLE hEmpty = OpenEmptySemaphore();
    if (!hEmpty) {
        std::wcout << L"OpenEmptySemaphore failed. Убедитесь, что Receiver создал семафор пустых слотов.\n";
        CloseHandle(hMutex);
        return 1;
    }

    HANDLE hFull = OpenFullSemaphore();
    if (!hFull) {
        std::wcout << L"OpenFullSemaphore failed. Убедитесь, что Receiver создал семафор заполненных слотов.\n";
        CloseHandle(hMutex);
        CloseHandle(hEmpty);
        return 1;
    }

    // Открываем событие готовности, которое Receiver создал заранее.
    HANDLE hReadyEvent = OpenSenderReadyEvent(senderIndex);
    if (!hReadyEvent) {
        std::wcout << L"OpenSenderReadyEvent failed. Убедитесь, что Receiver создал событие для этого индекса.\n";
        CloseHandle(hMutex);
        CloseHandle(hEmpty);
        CloseHandle(hFull);
        return 1;
    }

    if (!SetEvent(hReadyEvent)) {
        std::wcout << L"SetEvent(sender ready) failed\n";
        CloseHandle(hMutex);
        CloseHandle(hEmpty);
        CloseHandle(hFull);
        CloseHandle(hReadyEvent);
        return 1;
    }

    // Интерактивное меню для отправки сообщений
    std::wcout << L"Sender[" << senderIndex << L"] connected to queue: " << wfilename << L"\n";

    std::string line;
    while (true) {
        std::wcout << L"\n1 - send message\n2 - exit\n> ";
        int cmd;
        if (!(std::cin >> cmd)) {
            std::cin.clear();
            std::string dummy;
            std::getline(std::cin, dummy);
            continue;
        }
        std::getline(std::cin, line);

        if (cmd == 1) {
            DWORD waitRes = WaitForSingleObject(hEmpty, 0);
            if (waitRes == WAIT_TIMEOUT) {
                std::wcout << L"Нет свободных слотов. Sender завершает работу.\n";
                break;
            }
            else if (waitRes != WAIT_OBJECT_0) {
                std::wcout << L"Ошибка ожидания семафора empty. Код: " << GetLastError() << L"\n";
                break;
            }

            std::wcout << L"Enter message (max " << MAX_MSG_LEN - 1 << L" visible chars): ";
            std::string msg;
            std::getline(std::cin, msg);

            if ((int)msg.size() > MAX_MSG_LEN) msg.resize(MAX_MSG_LEN);

            waitRes = WaitForSingleObject(hMutex, INFINITE);
            if (waitRes != WAIT_OBJECT_0) {
                std::wcout << L"WaitForSingleObject(mutex) failed\n";
                ReleaseSemaphore(hEmpty, 1, nullptr);
                break;
            }

            bool ok = WriteMessage(wfilename, msg);
            ReleaseMutex(hMutex);

            if (!ok) {
                std::wcout << L"WriteMessage failed\n";
                ReleaseSemaphore(hEmpty, 1, nullptr);
                break;
            }

            if (!ReleaseSemaphore(hFull, 1, nullptr)) {
                std::wcout << L"ReleaseSemaphore(full) failed\n";
                break;
            }

            std::wcout << L"Message sent.\n";
        }
        else if (cmd == 2) {
            std::wcout << L"Exiting sender.\n";
            break;
        }
    }

    // Закрываем дескрипторы
    CloseHandle(hMutex);
    CloseHandle(hEmpty);
    CloseHandle(hFull);
    CloseHandle(hReadyEvent);
    return 0;
}
