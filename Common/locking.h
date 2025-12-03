#pragma once
#include <map>
#include <windows.h>

struct LockInfo
{
    int readerCount;
    bool isWriting;

    LockInfo() : readerCount(0), isWriting(false) {}
};

class LockManager
{
private:
    std::map<int, LockInfo> locks;
    HANDLE hMutex;

public:
    LockManager();
    ~LockManager();

    void lockForRead(int id);
    void unlockForRead(int id);

    void lockForWrite(int id);
    void unlockForWrite(int id);
};
