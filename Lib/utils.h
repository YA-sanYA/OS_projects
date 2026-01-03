#pragma once

#ifndef NOMINMAX
#define NOMINMAX // Запрещаем макросы min и max из windows.h
#endif

#include <windows.h>
#include <vector>
#include <iostream>
#include <limits>

struct ThreadData {
    std::vector<int> arr;
    int average = 0;
    int min_val = (std::numeric_limits<int>::max)();
    int max_val = (std::numeric_limits<int>::min)();

    friend std::istream& operator>>(std::istream& in, ThreadData& data);
    friend std::ostream& operator<<(std::ostream& out, const ThreadData& data);
};