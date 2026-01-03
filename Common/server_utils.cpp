#include "server_utils.h"
#include <iostream>

DWORD WINAPI handleClient(LPVOID param) {
    PipeContext* ctx = static_cast<PipeContext*>(param);
    HANDLE hPipe = ctx->hPipe;
    auto& db = ctx->database;
    LockManager& lm = *ctx->lockManager;

    Request req;
    Response res;
    int lockedId = -1;
    bool writing = false;
    DWORD cb;

    while (ReadFile(hPipe, &req, sizeof(req), &cb, NULL) && cb > 0) {
        if (req.type == RequestType::EXIT) break;

        if (req.type == RequestType::START_READ || req.type == RequestType::START_MODIFY) {
            if (req.type == RequestType::START_READ) lm.lockForRead(req.id);
            else lm.lockForWrite(req.id);

            employee* e = nullptr;
            for (auto& item : db) if (item.num == req.id) e = &item;

            if (e) {
                res.success = true;
                res.data = *e;
                lockedId = req.id;
                writing = (req.type == RequestType::START_MODIFY);
            }
            else {
                res.success = false;
                strcpy_s(res.message, "ID not found");
                if (req.type == RequestType::START_READ) lm.unlockForRead(req.id);
                else lm.unlockForWrite(req.id);
            }
            WriteFile(hPipe, &res, sizeof(res), &cb, NULL);
        }
        else if (req.type == RequestType::UPDATE_DATA) {
            if (lockedId == req.id && writing) {
                for (auto& item : db) if (item.num == req.id) item = req.data;
                res.success = true;
            }
            else {
                res.success = false;
                strcpy_s(res.message, "Access denied");
            }
            WriteFile(hPipe, &res, sizeof(res), &cb, NULL);
        }
        else if (req.type == RequestType::RELEASE) {
            if (lockedId != -1) {
                if (writing) lm.unlockForWrite(lockedId);
                else lm.unlockForRead(lockedId);
                lockedId = -1;
            }
        }
    }

    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    delete ctx;
    return 0;
}