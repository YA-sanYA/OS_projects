#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <io.h>
#include <fcntl.h>

#include "message_queue.h"
#include "sync_objects.h"

int wmain() {
    // Устанавливаем консоль в UTF-8
    SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_U8TEXT);  // для wcout
    _setmode(_fileno(stdin), _O_U8TEXT);  // для wcin

    // Ввод имени файла и количества записей
    std::wcout << L"Введите имя бинарного файла для очереди: ";
    std::wstring wfilename;
    std::getline(std::wcin, wfilename);
    if (wfilename.empty()) {
        std::wcout << L"Имя файла пустое.\n";
        return 1;
    }

    std::wcout << L"Введите количество записей (capacity, >0): ";
    int capacity = 0;
    {
        std::wstring line;
        std::getline(std::wcin, line);
        try { capacity = std::stoi(line); }
        catch (...) { capacity = 0; }
    }
    if (capacity <= 0) {
        std::wcout << L"Неправильное значение capacity.\n";
        return 1;
    }

    // Создаём бинарный файл очереди
    if (!InitMessageFile(wfilename, capacity)) {
        std::wcout << L"Не удалось создать/инициализировать файл: " << wfilename << L"\n";
        return 1;
    }
    std::wcout << L"Файл очереди создан: " << wfilename << L" (capacity=" << capacity << L")\n";

    // Ввод количества Sender'ов
    std::wcout << L"Введите количество процессов Sender: ";
    int numSenders = 0;
    {
        std::wstring line;
        std::getline(std::wcin, line);
        try { numSenders = std::stoi(line); }
        catch (...) { numSenders = 0; }
    }
    if (numSenders <= 0) {
        std::wcout << L"Неправильное количество Sender'ов.\n";
        return 1;
    }

    // Создаём синхро-объекты
    HANDLE hMutex = CreateMainMutex();
    HANDLE hEmpty = CreateEmptySemaphore(capacity, capacity);
    HANDLE hFull = CreateFullSemaphore(0, capacity);

    if (!hMutex || !hEmpty || !hFull) {
        std::wcout << L"Ошибка создания синхро-объектов.\n";
        if (hMutex) CloseHandle(hMutex);
        if (hEmpty) CloseHandle(hEmpty);
        if (hFull)  CloseHandle(hFull);
        return 1;
    }

    // Для каждого Sender создаём событие SenderReady_i
    std::vector<HANDLE> readyEvents;
    readyEvents.reserve(numSenders);
    for (int i = 0; i < numSenders; ++i) {
        HANDLE hev = CreateSenderReadyEvent(i);
        if (!hev) {
            std::wcout << L"Не удалось создать событие готовности для Sender " << i << L"\n";
            for (HANDLE h : readyEvents) CloseHandle(h);
            CloseHandle(hMutex); CloseHandle(hEmpty); CloseHandle(hFull);
            return 1;
        }
        readyEvents.push_back(hev);
    }

    // Запускаем заданное количество процессов Sender, передаём им имя файла и их индекс
    std::vector<PROCESS_INFORMATION> procs;
    procs.reserve(numSenders);
    std::wstring senderPath = L"C:\\Users\\Lenovo\\source\\repos\\OS_lab4\\out\\build\\x86-debug\\Sender\\Sender.exe";
    for (int i = 0; i < numSenders; ++i) {
        std::wstring cmd = L"\"" + senderPath + L"\" \"" + wfilename + L"\" " + std::to_wstring(i);

        std::vector<wchar_t> cmdline(cmd.begin(), cmd.end());
        cmdline.push_back(0);

        STARTUPINFOW si{};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi{};

        BOOL ok = CreateProcessW(
            nullptr,                
            cmdline.data(),
            nullptr, nullptr,
            FALSE,                  
            CREATE_NEW_CONSOLE,
            nullptr,                
            nullptr,                
            &si,
            &pi
        );

        if (!ok) {
            std::wcout << L"Не удалось запустить Sender " << i << L". Код ошибки: " << GetLastError() << L"\n";
            for (auto& p : procs) { CloseHandle(p.hProcess); CloseHandle(p.hThread); }
            for (HANDLE h : readyEvents) CloseHandle(h);
            CloseHandle(hMutex); CloseHandle(hEmpty); CloseHandle(hFull);
            return 1;
        }

        procs.push_back(pi);
        CloseHandle(pi.hThread);
    }

    std::wcout << L"Запущены все Sender'ы. Ждём их готовности...\n";

    // Ждём сигнал на готовность от всех Sender'ов
    DWORD waitAll = WaitForMultipleObjects(static_cast<DWORD>(readyEvents.size()), readyEvents.data(), TRUE, INFINITE);
    if (waitAll == WAIT_FAILED) {
        std::wcout << L"Ошибка ожидания готовности Sender'ов. Код: " << GetLastError() << L"\n";
        for (auto& p : procs) CloseHandle(p.hProcess);
        for (HANDLE h : readyEvents) CloseHandle(h);
        CloseHandle(hMutex); CloseHandle(hEmpty); CloseHandle(hFull);
        return 1;
    }

    for (HANDLE h : readyEvents) CloseHandle(h);
    readyEvents.clear();

    std::wcout << L"Все Sender'ы готовы. Ввод команд: 1 - читать сообщение, 2 - выйти\n";

    // Основной цикл — по команде с консоли читать сообщение или завершить работу
    for (;;) {
        std::wcout << L"> ";
        std::wstring cmdLine;
        if (!std::getline(std::wcin, cmdLine)) break;
        if (cmdLine.empty()) continue;

        std::wstringstream ss(cmdLine);
        int cmd = 0;
        ss >> cmd;

        if (cmd == 1) {
            DWORD wres = WaitForSingleObject(hFull, 0);
            if (wres != WAIT_OBJECT_0) {
                std::wcout << L"Очередь пуста или возникла ошибка ожидания семафора full\n";
                break;
            }

            wres = WaitForSingleObject(hMutex, INFINITE);
            if (wres != WAIT_OBJECT_0) {
                std::wcout << L"Ошибка взятия мьютекса: " << GetLastError() << L"\n";
                ReleaseSemaphore(hFull, 1, nullptr);
                break;
            }

            std::string message;
            bool ok = ReadMessage(wfilename, message);
            if (!ok) {
                std::wcout << L"Ошибка чтения сообщения из файла.\n";
                ReleaseMutex(hMutex);
                ReleaseSemaphore(hFull, 1, nullptr);
                break;
            }

            ReleaseMutex(hMutex);

            ReleaseSemaphore(hEmpty, 1, nullptr);

            std::wstring wmessage(message.begin(), message.end());

            std::wcout << L"Получено сообщение: " << wmessage << L"\n";

        }
        else if (cmd == 2) {
            std::wcout << L"Завершение Receiver...\n";
            break;
        }
        else {
            std::wcout << L"Неизвестная команда. Используйте 1 (читать) или 2 (выйти).\n";
        }
    }

    for (auto& pi : procs) {
        CloseHandle(pi.hProcess);
    }

    // закрываем синхро-объекты
    CloseHandle(hMutex);
    CloseHandle(hEmpty);
    CloseHandle(hFull);

    std::wcout << L"Receiver завершён.\n";
    return 0;
}
