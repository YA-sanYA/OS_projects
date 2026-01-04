#include "locking.h"

EmployeeLock::EmployeeLock() : readers(0), writing(false) {
    InitializeCriticalSection(&cs);
    InitializeConditionVariable(&cv);
}

EmployeeLock::~EmployeeLock() {
    DeleteCriticalSection(&cs);
}

LockManager::LockManager() {
    InitializeCriticalSection(&managerCS);
}

LockManager::~LockManager() {
    EnterCriticalSection(&managerCS);
    for (auto& pair : locks) {
        delete pair.second;
    }
    locks.clear();
    LeaveCriticalSection(&managerCS);
    DeleteCriticalSection(&managerCS);
}

EmployeeLock* LockManager::GetLock(int id) {
    EnterCriticalSection(&managerCS);

    if (locks.find(id) == locks.end()) {
        locks[id] = new EmployeeLock();
    }
    EmployeeLock* l = locks[id];

    LeaveCriticalSection(&managerCS);
    return l;
}

void LockManager::lockForRead(int id) {
    EmployeeLock* l = GetLock(id);
    EnterCriticalSection(&l->cs);

    while (l->writing) {
        SleepConditionVariableCS(&l->cv, &l->cs, INFINITE);
    }
    l->readers++;

    LeaveCriticalSection(&l->cs);
}

void LockManager::unlockForRead(int id) {
    EmployeeLock* l = GetLock(id);
    EnterCriticalSection(&l->cs);

    l->readers--;
    if (l->readers == 0) {
        WakeAllConditionVariable(&l->cv);
    }

    LeaveCriticalSection(&l->cs);
}

void LockManager::lockForWrite(int id) {
    EmployeeLock* l = GetLock(id);
    EnterCriticalSection(&l->cs);

    while (l->writing || l->readers > 0) {
        SleepConditionVariableCS(&l->cv, &l->cs, INFINITE);
    }
    l->writing = true;

    LeaveCriticalSection(&l->cs);
}

void LockManager::unlockForWrite(int id) {
    EmployeeLock* l = GetLock(id);
    EnterCriticalSection(&l->cs);

    l->writing = false;
    WakeAllConditionVariable(&l->cv);

    LeaveCriticalSection(&l->cs);
}