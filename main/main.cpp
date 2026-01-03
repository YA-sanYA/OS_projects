#include <iostream>
#include <vector>
#include "threads.h"
#include "utils.h"
#include "array_utils.h"
#include "ThreadClass.h"

int main() {
    setlocale(LC_ALL, "Russian");
    ThreadData data;

    try {
        std::cin >> data;

        ThreadClass tMinMax(min_max_win, &data, "MinMaxThread");
        ThreadClass tAverage(average_win, &data, "AverageThread");

        tMinMax.wait();
        tAverage.wait();

        replaceMinMaxWithAverage(data);

        std::cout << "\nResult:\n" << data;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}