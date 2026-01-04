#include "pch.h"
#include "threads.h"
#include "utils.h"
#include "array_utils.h"
#include "ThreadClass.h"
#include <vector>

// 1. Тест функции поиска min и max
TEST(ThreadUtilsTest, MinMaxFunction) {
    ThreadData data;
    data.arr = { 10, 5, 7, 3, 8 };

    ThreadClass tMinMax(min_max_win, &data, "min_max");
    tMinMax.wait();

    EXPECT_EQ(data.min_val, 3);
    EXPECT_EQ(data.max_val, 10);
}

// 2. Тест функции поиска среднего
TEST(ThreadUtilsTest, AverageFunction) {
    ThreadData data;
    data.arr = { 2, 4, 6, 8 };

    ThreadClass tAverage(average_win, &data, "average");
    tAverage.wait();

    EXPECT_EQ(data.average, 5);
}

// 3. Интеграционный тест: замена элементов
TEST(ArrayUtilsTest, ReplaceMinMaxWithAverage) {
    ThreadData data;
    data.arr = { 1, 5, 3, 9, 2 };

    ThreadClass tMinMax(min_max_win, &data, "min_max");
    ThreadClass tAverage(average_win, &data, "average");
    tMinMax.wait();
    tAverage.wait();

    replaceMinMaxWithAverage(data);

    EXPECT_EQ(data.arr[0], 4);
    EXPECT_EQ(data.arr[3], 4);
    EXPECT_EQ(data.arr[1], 5);
    EXPECT_EQ(data.arr[2], 3);
    EXPECT_EQ(data.arr[4], 2);
}

// 4. Простой тест min/max
TEST(ThreadUtilsTest, MinMaxFunction_Simple) {
    ThreadData data;
    data.arr = { 7, 2, 9 };

    ThreadClass tMinMax(min_max_win, &data, "min_max");
    tMinMax.wait();

    EXPECT_EQ(data.min_val, 2);
    EXPECT_EQ(data.max_val, 9);
}

// 5. Тест на одновременную работу потоков
TEST(ThreadUtilsTest, MinMaxAndAverageTogether) {
    ThreadData data;
    data.arr = { 1, 4, 2, 8 };

    ThreadClass tMinMax(min_max_win, &data, "min_max");
    ThreadClass tAverage(average_win, &data, "average");
    tMinMax.wait();
    tAverage.wait();

    EXPECT_EQ(data.min_val, 1);
    EXPECT_EQ(data.max_val, 8);
    EXPECT_EQ(data.average, 3);
}

// 6. Тест на массив из одинаковых элементов
TEST(ArrayUtilsTest, ReplaceMinMaxWithAverage_AllSame) {
    ThreadData data;
    data.arr = { 5, 5, 5, 5 };

    ThreadClass tMinMax(min_max_win, &data, "min_max");
    ThreadClass tAverage(average_win, &data, "average");
    tMinMax.wait();
    tAverage.wait();

    replaceMinMaxWithAverage(data);

    for (int x : data.arr) {
        EXPECT_EQ(x, 5);
    }
}