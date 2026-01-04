#include <iostream>
#include <thread>
#include <exception>
#include "threads.h"
#include "utils.h"
#include "array_utils.h"

int main() {
    setlocale(LC_ALL, "Russian");

    ThreadData data;

    try {
        std::cin >> data;
        std::thread tMinMax(min_max_logic, &data);
        std::thread tAverage(average_logic, &data);

        if (tMinMax.joinable()) {
            tMinMax.join();
        }
        if (tAverage.joinable()) {
            tAverage.join();
        }

        replaceMinMaxWithAverage(data);

        std::cout << "\nResult:\n" << data;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: \n" << e.what() << "\n";
        return 1;
    }

    return 0;
}