#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

const int TIME_TO_OUTPUT_STREAM = 100;
const int CHARS_TO_IGNORE = 1000000;
const int MARKER_SLEEP_TIME = 5;

// Данные для WinAPI версии
struct ThreadData {
    std::vector<int>* arr;
    int size;
    int id;
    CRITICAL_SECTION* cs;

    HANDLE hStartEvent;
    HANDLE hBlockEvent;
    HANDLE hContinueEvent;
    HANDLE hTerminateEvent;

    bool isActive = true;
};

// Данные для std::thread версии
struct ThreadDataForThreads {
    int id;
    std::vector<int>* arr;
    std::mutex* arrMutex;

    bool isActive = true;
    bool shouldContinue = false;
    bool shouldTerminate = false;
    bool blocked = false;

    std::condition_variable cv;
};

void writeArray(const std::vector<int>& arr);
int readNaturalNumber(const std::string& startMessage, const std::string& errorMessage);