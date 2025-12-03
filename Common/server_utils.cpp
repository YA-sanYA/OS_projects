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

void handleClient(HANDLE hPipe,
    std::vector<employee>& database,
    LockManager& lockManager)
{
    Request req;
    Response res;

    while (true)
    {
        DWORD readBytes = 0;

        if (!ReadFile(hPipe, &req, sizeof(req), &readBytes, NULL))
            break;

        if (req.type == RequestType::EXIT)
            break;

        if (req.type == RequestType::READ)
        {
            lockManager.lockForRead(req.id);

            employee* e = findEmployee(database, req.id);

            if (e)
            {
                res.success = true;
                res.data = *e;
            }
            else
            {
                res.success = false;
            }

            WriteFile(hPipe, &res, sizeof(res), nullptr, nullptr);
        }

        if (req.type == RequestType::WRITE)
        {
            lockManager.lockForWrite(req.id);

            employee* e = findEmployee(database, req.id);

            if (e)
            {
                *e = req.data;
                res.success = true;
                res.data = *e;
            }
            else
            {
                res.success = false;
            }

            WriteFile(hPipe, &res, sizeof(res), nullptr, nullptr);
        }

        if (req.type == RequestType::RELEASE)
        {
            lockManager.unlockForRead(req.id);
            lockManager.unlockForWrite(req.id);
        }
    }

    CloseHandle(hPipe);
}
