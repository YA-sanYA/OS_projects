#include "locking.h"

LockManager::LockManager()
{
    hMutex = CreateMutex(NULL, FALSE, NULL);
}

LockManager::~LockManager()
{
    CloseHandle(hMutex);
}

void LockManager::lockForRead(int id)
{
    while (true)
    {
        WaitForSingleObject(hMutex, INFINITE);

        if (!locks[id].isWriting)
        {
            locks[id].readerCount++;
            ReleaseMutex(hMutex);
            break;
        }

        ReleaseMutex(hMutex);
        Sleep(100);
    }
}

void LockManager::unlockForRead(int id)
{
    WaitForSingleObject(hMutex, INFINITE);
    if (locks[id].readerCount > 0)
        locks[id].readerCount--;
    ReleaseMutex(hMutex);
}

void LockManager::lockForWrite(int id)
{
    while (true)
    {
        WaitForSingleObject(hMutex, INFINITE);

        if (!locks[id].isWriting && locks[id].readerCount == 0)
        {
            locks[id].isWriting = true;
            ReleaseMutex(hMutex);
            break;
        }

        ReleaseMutex(hMutex);
        Sleep(100);
    }
}

void LockManager::unlockForWrite(int id)
{
    WaitForSingleObject(hMutex, INFINITE);
    locks[id].isWriting = false;
    ReleaseMutex(hMutex);
}
