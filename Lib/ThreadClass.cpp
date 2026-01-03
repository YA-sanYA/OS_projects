#include "ThreadClass.h"
#include <stdexcept>

ThreadClass::ThreadClass(LPTHREAD_START_ROUTINE func, LPVOID lpParam, const std::string& name)
    : name(name), hThread(NULL)
{
    hThread = CreateThread(NULL, 0, func, lpParam, 0, NULL);
    if (hThread == NULL) {
        throw std::runtime_error("Failed to create thread: " + name);
    }
}

void ThreadClass::wait() {
    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
    }
}

ThreadClass::~ThreadClass() {
    if (hThread) {
        CloseHandle(hThread);
        hThread = NULL;
    }
}