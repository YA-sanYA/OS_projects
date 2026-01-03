#include "Threads.h"
#include <iostream>
#include <vector>

DWORD WINAPI marker(LPVOID lpParam) {
    ThreadData* data = static_cast<ThreadData*>(lpParam);
    if (!data) return 1;

    WaitForSingleObject(data->hStartEvent, INFINITE);
    srand(data->id);

    std::vector<int> markedIndices;

    while (true) {
        int index = rand() % data->size;

        EnterCriticalSection(data->cs);
        int val = (*data->arr)[index];
        LeaveCriticalSection(data->cs);

        if (val == 0) {
            Sleep(MARKER_SLEEP_TIME);

            EnterCriticalSection(data->cs);
            if ((*data->arr)[index] == 0) {
                (*data->arr)[index] = data->id;
                markedIndices.push_back(index);
                LeaveCriticalSection(data->cs);
                Sleep(MARKER_SLEEP_TIME);
            }
            else {
                LeaveCriticalSection(data->cs);
            }
        }
        else {
            EnterCriticalSection(data->cs);
            std::cout << "\n[Thread " << data->id << "] Blocked at index: " << index
                << ". Elements marked: " << markedIndices.size() << std::endl;
            LeaveCriticalSection(data->cs);

            SetEvent(data->hBlockEvent);

            HANDLE waitHandles[2] = { data->hContinueEvent, data->hTerminateEvent };
            DWORD waitRes = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);

            if (waitRes == WAIT_OBJECT_0) {
                ResetEvent(data->hContinueEvent);
                continue;
            }
            else {
                EnterCriticalSection(data->cs);
                for (int idx : markedIndices) {
                    (*data->arr)[idx] = 0;
                }
                LeaveCriticalSection(data->cs);
                break;
            }
        }
    }
    return 0;
}