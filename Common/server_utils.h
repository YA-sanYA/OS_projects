#pragma once
#include <windows.h>
#include "common.h"
#include "locking.h"
#include <vector>

employee* findEmployee(std::vector<employee>& database, int id);

void handleClient(
    HANDLE hPipe,
    std::vector<employee>& database,
    LockManager& lockManager
);
