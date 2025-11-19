#pragma once
#include <windows.h>

// Глобальные имена
inline constexpr wchar_t MUTEX_NAME[] = L"Global\\MsgQueue_Mutex";
inline constexpr wchar_t SEM_FULL_NAME[] = L"Global\\MsgQueue_Full";
inline constexpr wchar_t SEM_EMPTY_NAME[] = L"Global\\MsgQueue_Empty";

// Создание объектов (выполняет только Receiver)
HANDLE CreateMainMutex();
HANDLE CreateFullSemaphore(int initialCount, int maxCount);
HANDLE CreateEmptySemaphore(int initialCount, int maxCount);
HANDLE CreateSenderReadyEvent(int senderIndex);

// Открытие существующих объектов (Sender)
HANDLE OpenMainMutex();
HANDLE OpenFullSemaphore();
HANDLE OpenEmptySemaphore();
HANDLE OpenSenderReadyEvent(int senderIndex);
