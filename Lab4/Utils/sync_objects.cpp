#include "sync_objects.h"
#include <string>

// (Receiver вызывает ЭТИ функции)
HANDLE CreateMainMutex() {
    return CreateMutexW(
        nullptr,
        FALSE,
        MUTEX_NAME
    );
}

HANDLE CreateFullSemaphore(int initialCount, int maxCount) {
    return CreateSemaphoreW(
        nullptr,
        initialCount,
        maxCount,
        SEM_FULL_NAME
    );
}

HANDLE CreateEmptySemaphore(int initialCount, int maxCount) {
    return CreateSemaphoreW(
        nullptr,
        initialCount,
        maxCount,
        SEM_EMPTY_NAME
    );
}

HANDLE CreateSenderReadyEvent(int senderIndex) {
    std::wstring name = L"Global\\SenderReady_" + std::to_wstring(senderIndex);

    return CreateEventW(
        nullptr,
        TRUE,
        FALSE,
        name.c_str()
    );
}

// (Sender вызывает ЭТИ функции)
HANDLE OpenMainMutex() {
    return OpenMutexW(SYNCHRONIZE, FALSE, MUTEX_NAME);
}

HANDLE OpenFullSemaphore() {
    return OpenSemaphoreW(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
        FALSE,
        SEM_FULL_NAME);
}

HANDLE OpenEmptySemaphore() {
    return OpenSemaphoreW(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
        FALSE,
        SEM_EMPTY_NAME);
}

HANDLE OpenSenderReadyEvent(int senderIndex) {
    std::wstring name = L"Global\\SenderReady_" + std::to_wstring(senderIndex);
    return OpenEventW(EVENT_MODIFY_STATE | SYNCHRONIZE,
        FALSE,
        name.c_str());
}
