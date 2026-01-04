#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "common.h"
#include "locking.h"

struct PipeContext {
    HANDLE hPipe;
    std::vector<employee>& database;
    LockManager* lockManager;
};

void createBinaryFile(const std::string& filename, std::vector<employee>& database);
void saveFile(const std::string& filename, const std::vector<employee>& database);
void printFile(const std::string& filename);

HANDLE createServerPipe();
void startClients(int clientCount, const std::wstring& clientPath);