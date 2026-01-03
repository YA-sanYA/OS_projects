#include "utils.h"
#include <stdexcept>
#include <iomanip>

void writeArray(const std::vector<int>& arr) {
    std::cout << "Содержимое массива: ";
    for (int x : arr) {
        std::cout << x << " ";
    }
    std::cout << "\n" << std::endl;
}

int readNaturalNumber(const std::string& startMessage, const std::string& errorMessage) {
    int res;
    std::cout << startMessage;

    if (!(std::cin >> res)) {
        std::cin.clear();
        std::cin.ignore(CHARS_TO_IGNORE, '\n');
        throw std::invalid_argument("Ошибка ввода: введите целое число.\n");
    }

    if (res <= 0) {
        throw std::out_of_range(errorMessage);
    }

    return res;
}