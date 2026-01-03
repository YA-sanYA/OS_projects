#include "OS_lab3.h"
#include <algorithm>

int main() {
    setlocale(LC_ALL, "Russian");
    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);

    try {
        int arraySize = readNaturalNumber("Введите размер массива: ", "Размер должен быть > 0");
        std::vector<int> array(arraySize, 0);

        int threadCount = readNaturalNumber("Введите количество потоков marker: ", "Кол-во должно быть > 0");

        if (threadCount > MAXIMUM_WAIT_OBJECTS) {
            throw std::runtime_error("Количество потоков не может превышать 64 (лимит Windows).");
        }

        HANDLE hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        std::vector<ThreadData> threadData(threadCount);
        std::vector<HANDLE> hThreads(threadCount);

        for (int i = 0; i < threadCount; i++) {
            threadData[i].arr = &array;
            threadData[i].size = arraySize;
            threadData[i].id = i + 1;
            threadData[i].cs = &cs;
            threadData[i].hStartEvent = hStartEvent;

            threadData[i].hBlockEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            threadData[i].hContinueEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            threadData[i].hTerminateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

            hThreads[i] = CreateThread(NULL, 0, marker, &threadData[i], 0, NULL);
        }

        SetEvent(hStartEvent);

        int activeCount = threadCount;
        while (activeCount > 0) {
            std::vector<HANDLE> hWaitEvents;
            for (auto& td : threadData) {
                if (td.isActive) hWaitEvents.push_back(td.hBlockEvent);
            }

            WaitForMultipleObjects((DWORD)hWaitEvents.size(), hWaitEvents.data(), TRUE, INFINITE);

            writeArray(array);

            int idToKill;
            std::cout << "Введите ID потока для завершения: ";
            std::cin >> idToKill;

            if (idToKill < 1 || idToKill > threadCount || !threadData[idToKill - 1].isActive) {
                std::cout << "Некорректный ID или поток уже мертв." << std::endl;
                continue;
            }

            int idx = idToKill - 1;
            SetEvent(threadData[idx].hTerminateEvent);
            WaitForSingleObject(hThreads[idx], INFINITE);

            threadData[idx].isActive = false;
            activeCount--;

            writeArray(array);

            for (auto& td : threadData) {
                if (td.isActive) {
                    ResetEvent(td.hBlockEvent);
                    SetEvent(td.hContinueEvent);
                }
            }
        }

        for (int i = 0; i < threadCount; i++) {
            CloseHandle(hThreads[i]);
            CloseHandle(threadData[i].hBlockEvent);
            CloseHandle(threadData[i].hContinueEvent);
            CloseHandle(threadData[i].hTerminateEvent);
        }
        CloseHandle(hStartEvent);
        DeleteCriticalSection(&cs);

    }
    catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        DeleteCriticalSection(&cs);
        return 1;
    }

    std::cout << "Все потоки завершены. Программа окончена." << std::endl;
    return 0;
}