#include "testUtils.h"
#include <algorithm>

struct HandleWrapper {
    HANDLE h = NULL;
    ~HandleWrapper() { if (h) CloseHandle(h); }
};

void RunMarkerThreadsTest(int arraySize, int threadNumber)
{
    if (threadNumber > 64) throw std::invalid_argument("Too many threads for WinAPI");

    std::vector<int> array(arraySize, 0);
    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);

    HandleWrapper hStartEvent;
    hStartEvent.h = CreateEvent(NULL, TRUE, FALSE, NULL);

    std::vector<ThreadData> threadData(threadNumber);
    std::vector<HandleWrapper> threadHandles(threadNumber);
    std::vector<HandleWrapper> blockEvents(threadNumber);
    std::vector<HandleWrapper> continueEvents(threadNumber);
    std::vector<HandleWrapper> terminateEvents(threadNumber);

    for (int i = 0; i < threadNumber; i++) {
        threadData[i].arr = &array;
        threadData[i].size = arraySize;
        threadData[i].id = i + 1;
        threadData[i].cs = &cs;
        threadData[i].hStartEvent = hStartEvent.h;

        blockEvents[i].h = CreateEvent(NULL, TRUE, FALSE, NULL);
        continueEvents[i].h = CreateEvent(NULL, TRUE, FALSE, NULL);
        terminateEvents[i].h = CreateEvent(NULL, TRUE, FALSE, NULL);

        threadData[i].hBlockEvent = blockEvents[i].h;
        threadData[i].hContinueEvent = continueEvents[i].h;
        threadData[i].hTerminateEvent = terminateEvents[i].h;

        threadHandles[i].h = CreateThread(NULL, 0, marker, &threadData[i], 0, NULL);
    }

    SetEvent(hStartEvent.h);

    for (int i = 0; i < threadNumber; i++) {
        std::vector<HANDLE> activeBlockEvents;
        for (int j = 0; j < threadNumber; j++) {
            if (threadData[j].isActive) activeBlockEvents.push_back(threadData[j].hBlockEvent);
        }

        if (!activeBlockEvents.empty()) {
            WaitForMultipleObjects((DWORD)activeBlockEvents.size(), activeBlockEvents.data(), TRUE, INFINITE);
        }

        EnterCriticalSection(&cs);
        for (int val : array) {
            bool isValid = (val == 0) || (val >= 1 && val <= threadNumber);
            if (!isValid) {
                LeaveCriticalSection(&cs);
                DeleteCriticalSection(&cs);
                throw std::runtime_error("Array contains corrupted data!");
            }
        }
        LeaveCriticalSection(&cs);

        int targetIdx = -1;
        for (int j = 0; j < threadNumber; j++) {
            if (threadData[j].isActive) { targetIdx = j; break; }
        }

        if (targetIdx != -1) {
            SetEvent(threadData[targetIdx].hTerminateEvent);
            WaitForSingleObject(threadHandles[targetIdx].h, INFINITE);
            threadData[targetIdx].isActive = false;

            EnterCriticalSection(&cs);
            int id = targetIdx + 1;
            auto it = std::find(array.begin(), array.end(), id);
            bool found = (it != array.end());
            LeaveCriticalSection(&cs);

            if (found) {
                DeleteCriticalSection(&cs);
                throw std::runtime_error("Thread died but didn't clean up its marks!");
            }
        }

        for (int j = 0; j < threadNumber; j++) {
            if (threadData[j].isActive) {
                ResetEvent(threadData[j].hBlockEvent);
                SetEvent(threadData[j].hContinueEvent);
            }
        }
    }

    DeleteCriticalSection(&cs);
}