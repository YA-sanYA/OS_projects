#include "utils.h"

std::istream& operator>>(std::istream& in, ThreadData& data) {
    int size;
    std::cout << "Enter array size: ";
    if (!(in >> size) || size <= 0) {
        throw std::runtime_error("Invalid array size");
    }

    data.arr.resize(size);
    std::cout << "Enter " << size << " elements: ";
    for (int i = 0; i < size; ++i) {
        if (!(in >> data.arr[i])) {
            throw std::runtime_error("Invalid element input");
        }
    }
    return in;
}

std::ostream& operator<<(std::ostream& out, const ThreadData& data) {
    out << "Array: ";
    for (int x : data.arr) {
        out << x << " ";
    }
    out << std::endl;
    return out;
}