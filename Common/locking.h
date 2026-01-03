#pragma once
#include <map>
#include <windows.h>

// Вспомогательная структура для конкретного ID
struct EmployeeLock {
    int readers;
    bool writing;
    CRITICAL_SECTION cs;
    CONDITION_VARIABLE cv;

    EmployeeLock();
    ~EmployeeLock();
};

class LockManager {
private:
    std::map<int, EmployeeLock*> locks;
    CRITICAL_SECTION managerCS; // Защита самой коллекции (map)

    // Внутренний метод получения или создания блокировки
    EmployeeLock* GetLock(int id);

public:
    LockManager();
    ~LockManager();

    // Блокировка/разблокировка для чтения (множественный доступ)
    void lockForRead(int id);
    void unlockForRead(int id);

    // Блокировка/разблокировка для записи (эксклюзивный доступ)
    void lockForWrite(int id);
    void unlockForWrite(int id);
};