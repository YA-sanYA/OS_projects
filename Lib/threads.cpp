#include "threads.h"

void min_max_logic(ThreadData* data) {
    if (!data || data->arr.empty()) return;

    for (int x : data->arr) {
        if (x < data->min_val) {
            data->min_val = x;
        }
        Sleep(7);

        if (x > data->max_val) {
            data->max_val = x;
        }
        Sleep(7);
    }
    std::cout << "[min_max] Min: " << data->min_val << ", Max: " << data->max_val << std::endl;
}

void average_logic(ThreadData* data) {
    if (!data || data->arr.empty()) return;

    long long sum = 0;
    for (int x : data->arr) {
        sum += x;
        Sleep(12);
    }

    data->average = static_cast<int>(sum / data->arr.size());
    std::cout << "[average] Average: " << data->average << std::endl;
}

// Обертки просто вызывают логику
DWORD WINAPI min_max_win(LPVOID lpParam) {
    min_max_logic(static_cast<ThreadData*>(lpParam));
    return 0;
}

DWORD WINAPI average_win(LPVOID lpParam) {
    average_logic(static_cast<ThreadData*>(lpParam));
    return 0;
}