#pragma once
#include <windows.h>
#include <string>

inline constexpr wchar_t MUTEX_NAME[] = L"Local\\MsgQueue_Mutex";
inline constexpr wchar_t SEM_FULL_NAME[] = L"Local\\MsgQueue_Full";
inline constexpr wchar_t SEM_EMPTY_NAME[] = L"Local\\MsgQueue_Empty";

HANDLE CreateMainMutex();
HANDLE CreateFullSemaphore(int initialCount, int maxCount);
HANDLE CreateEmptySemaphore(int initialCount, int maxCount);
HANDLE CreateSenderReadyEvent(int senderIndex);

HANDLE OpenMainMutex();
HANDLE OpenFullSemaphore();
HANDLE OpenEmptySemaphore();
HANDLE OpenSenderReadyEvent(int senderIndex);