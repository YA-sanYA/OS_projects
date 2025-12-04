#include "server_utils.h"
#include <iostream>

employee* findEmployee(std::vector<employee>& database, int id)
{
    for (auto& e : database)
    {
        if (e.num == id)
            return &e;
    }
    return nullptr;
}

void handleClient(HANDLE hPipe, std::vector<employee>& database, LockManager& lockManager)
{
    Request req;
    Response res;
    int lockedId = -1;
    bool isWriteLock = false;

    while (true)
    {
        DWORD readBytes = 0;
        BOOL result = ReadFile(hPipe, &req, sizeof(req), &readBytes, NULL);

        if (!result || readBytes == 0 || req.type == RequestType::EXIT)
        {
            if (lockedId != -1) {
                if (isWriteLock) lockManager.unlockForWrite(lockedId);
                else lockManager.unlockForRead(lockedId);
            }
            break;
        }

        // 1. ЗАПРОС НА ЧТЕНИЕ
        if (req.type == RequestType::START_READ)
        {
            lockManager.lockForRead(req.id);

            employee* e = findEmployee(database, req.id);
            if (e) {
                res.success = true;
                res.data = *e;
                lockedId = req.id;
                isWriteLock = false;
            }
            else {
                res.success = false;
                lockManager.unlockForRead(req.id);
                strcpy_s(res.message, "ID not found");
            }
            WriteFile(hPipe, &res, sizeof(res), nullptr, nullptr);
        }

        // 2. ЗАПРОС НА МОДИФИКАЦИЮ
        else if (req.type == RequestType::START_MODIFY)
        {
            lockManager.lockForWrite(req.id);

            employee* e = findEmployee(database, req.id);
            if (e) {
                res.success = true;
                res.data = *e;
                lockedId = req.id;
                isWriteLock = true;
            }
            else {
                res.success = false;
                lockManager.unlockForWrite(req.id);
                strcpy_s(res.message, "ID not found");
            }
            WriteFile(hPipe, &res, sizeof(res), nullptr, nullptr);
        }

        // 3. ОБНОВЛЕНИЕ ДАННЫХ (только если уже заблокировано)
        else if (req.type == RequestType::UPDATE_DATA)
        {
            if (lockedId == req.id && isWriteLock)
            {
                employee* e = findEmployee(database, req.id);
                if (e) {
                    *e = req.data;
                    res.success = true;
                }
                else {
                    res.success = false; // Странная ситуация, удалили пока редактировали?
                }
            }
            else {
                res.success = false;
                strcpy_s(res.message, "Not locked for write!");
            }
            WriteFile(hPipe, &res, sizeof(res), nullptr, nullptr);
        }

        // 4. СНЯТИЕ БЛОКИРОВКИ
        else if (req.type == RequestType::RELEASE)
        {
            if (lockedId != -1)
            {
                if (isWriteLock)
                    lockManager.unlockForWrite(lockedId);
                else
                    lockManager.unlockForRead(lockedId);

                lockedId = -1;
            }
            res.success = true;
        }
    }

    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
}