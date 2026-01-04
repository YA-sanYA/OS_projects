#include "array_utils.h"

void replaceMinMaxWithAverage(ThreadData& data) {
    for (size_t i = 0; i < data.arr.size(); i++) {
        if (data.arr[i] == data.min_val || data.arr[i] == data.max_val) {
            data.arr[i] = data.average;
        }
    }
}